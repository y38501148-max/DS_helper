use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::env;
use std::fs;
use std::io::{Read, Write};
use std::path::PathBuf;
use std::process::{Command, Stdio};
use std::thread;
use std::time::{Duration, Instant, SystemTime, UNIX_EPOCH};

const APP_DATA_JSON: &str = include_str!("../resources/ds_helper_data.json");

#[derive(Clone, Deserialize, Serialize)]
struct CodeTemplate {
    name: String,
    code: String,
}

#[derive(Clone, Deserialize, Serialize)]
struct ToolVariant {
    name: String,
    code: String,
    usage: String,
}

#[derive(Clone, Deserialize, Serialize)]
struct FillTool {
    name: String,
    variants: Vec<ToolVariant>,
}

#[derive(Clone, Deserialize, Serialize)]
struct AppData {
    templates: Vec<CodeTemplate>,
    tools: Vec<FillTool>,
}

struct ProcessOutput {
    exit_code: Option<i32>,
    timeout: bool,
    stdout: String,
    stderr: String,
}

fn app_data() -> Result<AppData, String> {
    serde_json::from_str(APP_DATA_JSON).map_err(|error| format!("无法解析内嵌模板数据：{error}"))
}

#[tauri::command]
fn get_app_data() -> Result<Value, String> {
    serde_json::from_str(APP_DATA_JSON).map_err(|error| format!("无法解析内嵌模板数据：{error}"))
}

fn find_gcc() -> PathBuf {
    #[cfg(windows)]
    {
        if let Ok(exe) = env::current_exe() {
            if let Some(dir) = exe.parent() {
                let beside = dir.join("mingw64").join("bin").join("gcc.exe");
                if beside.exists() {
                    return beside;
                }
            }
        }
        PathBuf::from("gcc.exe")
    }

    #[cfg(not(windows))]
    {
        PathBuf::from("gcc")
    }
}

fn read_pipe<T: Read + Send + 'static>(mut pipe: T) -> thread::JoinHandle<String> {
    thread::spawn(move || {
        let mut buffer = String::new();
        let _ = pipe.read_to_string(&mut buffer);
        buffer
    })
}

fn run_process(mut command: Command, stdin_text: &str, timeout: Duration) -> Result<ProcessOutput, String> {
    let mut child = command
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn()
        .map_err(|error| format!("无法启动进程：{error}"))?;

    if let Some(mut stdin) = child.stdin.take() {
        stdin
            .write_all(stdin_text.as_bytes())
            .map_err(|error| format!("无法写入 stdin：{error}"))?;
    }

    let stdout_thread = child
        .stdout
        .take()
        .map(read_pipe)
        .ok_or_else(|| "无法捕获 stdout".to_string())?;
    let stderr_thread = child
        .stderr
        .take()
        .map(read_pipe)
        .ok_or_else(|| "无法捕获 stderr".to_string())?;

    let start = Instant::now();
    let mut timeout_reached = false;
    let status = loop {
        if let Some(status) = child.try_wait().map_err(|error| format!("无法等待进程：{error}"))? {
            break status;
        }
        if start.elapsed() > timeout {
            let _ = child.kill();
            timeout_reached = true;
            break child
                .wait()
                .map_err(|error| format!("无法终止超时进程：{error}"))?;
        }
        thread::sleep(Duration::from_millis(20));
    };

    let stdout = stdout_thread.join().unwrap_or_default();
    let stderr = stderr_thread.join().unwrap_or_default();

    Ok(ProcessOutput {
        exit_code: status.code(),
        timeout: timeout_reached,
        stdout,
        stderr,
    })
}

fn temp_work_dir() -> Result<PathBuf, String> {
    let stamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map_err(|error| format!("系统时间异常：{error}"))?
        .as_millis();
    let dir = env::temp_dir().join(format!("DS_helper_{}_{}", std::process::id(), stamp));
    fs::create_dir_all(&dir).map_err(|error| format!("无法创建临时目录：{error}"))?;
    Ok(dir)
}

fn run_c_program(code: &str, stdin_text: &str) -> Result<String, String> {
    let dir = temp_work_dir()?;
    let source_path = dir.join("main.c");
    let exe_path = dir.join(if cfg!(windows) { "tool.exe" } else { "tool" });
    fs::write(&source_path, code).map_err(|error| format!("无法写入临时代码文件：{error}"))?;

    let mut compile = Command::new(find_gcc());
    compile
        .arg("-std=c11")
        .arg("-O2")
        .arg("-Wall")
        .arg("-Wextra")
        .arg("-finput-charset=UTF-8")
        .arg("-fexec-charset=UTF-8");
    if cfg!(windows) {
        compile.arg("-static-libgcc");
    }
    compile.arg(&source_path).arg("-o").arg(&exe_path).current_dir(&dir);

    let compile_output = run_process(compile, "", Duration::from_secs(10))?;
    if compile_output.timeout || compile_output.exit_code != Some(0) {
        let _ = fs::remove_file(&source_path);
        let _ = fs::remove_dir(&dir);
        let mut message = String::from("编译失败：\n");
        message.push_str(&compile_output.stderr);
        message.push_str(&compile_output.stdout);
        if compile_output.timeout {
            message.push_str("\n编译超时。");
        }
        return Ok(message);
    }

    let mut run = Command::new(&exe_path);
    run.current_dir(&dir);
    let run_output = run_process(run, stdin_text, Duration::from_secs(5))?;
    let mut output = run_output.stdout;
    if !run_output.stderr.is_empty() {
        if !output.is_empty() {
            output.push('\n');
        }
        output.push_str(&run_output.stderr);
    }
    if run_output.timeout {
        if !output.is_empty() {
            output.push('\n');
        }
        output.push_str("运行超时，进程已终止。");
    }
    if output.is_empty() {
        output = "程序没有输出。".to_string();
    }

    let _ = fs::remove_file(&exe_path);
    let _ = fs::remove_file(&source_path);
    let _ = fs::remove_dir(&dir);
    Ok(output)
}

#[tauri::command]
fn run_tool(tool_index: usize, variant_index: usize, stdin_text: String) -> Result<String, String> {
    let data = app_data()?;
    let tool = data
        .tools
        .get(tool_index)
        .ok_or_else(|| "工具索引无效。".to_string())?;
    let variant = tool
        .variants
        .get(variant_index)
        .ok_or_else(|| "功能索引无效。".to_string())?;
    run_c_program(&variant.code, &stdin_text)
}

pub fn run() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![get_app_data, run_tool])
        .run(tauri::generate_context!())
        .expect("failed to run DS_helper");
}
