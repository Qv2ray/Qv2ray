import * as process from "process";
import * as glob from "glob";
import * as core from '@actions/core';
import * as exec from '@actions/exec';

async function run() {
  try {
    // 7-zip is required, and not included on macOS
    if (process.platform == "darwin") {
      await exec.exec("brew install p7zip")
    }

    await exec.exec("pip3 install aqtinstall");

    const dir = core.getInput("dir") || process.env.RUNNER_WORKSPACE;
    const version = core.getInput("version");
    let host = core.getInput("host");
    let target = core.getInput("target");
    let arch = core.getInput("arch");

    //set host automatically if omitted
    if (!host) {
      switch(process.platform) {
        case "win32": {
            host = "windows";
            break;
        }
        case "darwin": {
            host = "mac";
            break;
        }
        default: {
            host = "linux";
            break;
        }
      }
    }

    //set arch automatically if omitted
    if (!arch) {
      if (host == "windows") {
        arch = "win64_mingw73";
      } else if (host == "android") {
        arch = "android_armv7";
      }
    }

    //set args
    let args = ["-O", `${dir}`, `${version}`, `${host}`, `${target}`];
    if (arch) {
      args.push(`${arch}`);
    }

    //accomodate for differences in python 3 executable name
    let pythonName = "python3";
    if (process.platform == "win32") {
      pythonName = "python";
    }

    //run aqtinstall with args
    await exec.exec(`${pythonName} -m aqt install`, args);

    //set environment variables
    let qtPath = dir + "/Qt" + version + "/" + version;
    qtPath = glob.sync(qtPath + '/**/*')[0];

    core.exportVariable('Qt5_Dir', qtPath);
    core.addPath(qtPath + "/bin");
    
  } catch (error) {
    core.setFailed(error.message);
  }
}

run();
