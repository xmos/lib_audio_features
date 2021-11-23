@Library('xmos_jenkins_shared_library@v0.16.4') _

pipeline {
  agent {
    label "xcore.ai"
  }
  parameters {
    string(
      name: 'TOOLS_VERSION',
      defaultValue: '15.0.4',
      description: 'The tools version to build with (check /projects/tools/ReleasesTools/)'
    )
  }
  environment {
    // '/XMOS/tools' from get_tools.py and rest from tools installers
    TOOLS_PATH = "/XMOS/tools/${params.TOOLS_VERSION}/XMOS/xTIMEcomposer/${params.TOOLS_VERSION}"
  }
  options {
    skipDefaultCheckout()
  }
  stages {
    stage('Checkout') {
      steps {
        checkout scm
        sh "git submodule update --jobs 8 --init --recursive"
        sh "git clone -b v1.5.0 git@github0.xmos.com:xmos-int/xtagctl.git"
        //Note shallow clone of AIOT to speed up regression
	    sh "git clone --depth 1 --jobs 8 --recurse-submodules git@github.com:xmos/aiot_sdk.git"
        sh "cd aiot_sdk/modules/lib_xs3_math && git checkout develop && cd -"
        //Manually copy over setup file so we can access python modules from 
        sh "cp tests/bringup_py/setup.py aiot_sdk/tools/ai_tools/third_party/tensorflow/tensorflow/examples/"
      }
    }
    stage('Install Dependencies') {
      steps {
        sh '/XMOS/get_tools.py 15.0.4'
        toolsEnv(TOOLS_PATH) {  // load xmos tools
          installDependencies()
        }
      }
    }
    stage('Static analysis') {
      steps {
        withVenv() {
          sh "flake8 --exit-zero --output-file=flake8.xml lib_voice_front_end"
          recordIssues enabledForFailure: true, tool: flake8(pattern: 'flake8.xml')
        }
      }
    }
    stage('Build') {
      steps {
        toolsEnv(TOOLS_PATH) {  // load xmos tools
          withVenv() {
            sh 'python aiot_sdk/modules/lib_xs3_math/lib_xs3_math/script/gen_fft_table.py --dit --max_fft_log2 10 --out_dir aiot_sdk/modules/lib_xs3_math/lib_xs3_math/src/vect'
            // sh 'python aiot_sdk/modules/lib_xs3_math/lib_xs3_math/script/gen_fft_table.py --dif --max_fft_log2 10 --out_dir aiot_sdk/modules/lib_xs3_math/lib_xs3_math/src/vect'
          }
        }
      }
    }
    stage('Tests'){
      failFast false
      parallel {
        stage('Hardware tests') {
          stages{
            stage('Cleanup xtagctl'){
              steps {
                sh 'rm -f ~/.xtag/status.lock ~/.xtag/acquired'
              }
            }
            stage('Placeholder test'){
              steps {
                dir('tests') {
                  withVenv() {
                    toolsEnv(TOOLS_PATH) {
                      sh 'echo "hello world"'
                    }
                  }
                }
              }
            }
          }
        }
        stage('xsim tests'){
          stages{
            stage('Model to Xcore equivalence'){
              steps {
                dir('tests/equivalence') {
                  withVenv() {
                    toolsEnv(TOOLS_PATH) {
                      withEnv(["DISPLAY=none", "XMOS_AIOT_SDK_PATH=../../aiot_sdk"]) {
                        sh 'python -m pytest --junitxml=pytest_result.xml -s'
                        junit 'pytest_result.xml'
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  post {
    success {
      println "Finished"
    }
    // always {
    //   archiveArtifacts artifacts: "tests/pipelines/*.csv", fingerprint: true, allowEmptyArchive: true
    // }
    cleanup {
      cleanWs()
    }
  }
}
