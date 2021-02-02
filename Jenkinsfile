@Library('xmos_jenkins_shared_library@v0.16.0') _

pipeline {
  agent {
    label "xcore.ai-explorer"
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
        sh "git submodule update --jobs 4 --init --recursive"
        sh "git clone -b v1.1.3 git@github0.xmos.com:xmos-int/xtagctl.git"
        sh "git clone git@github.com:xmos/lib_xs3_math.git"
        sh "git clone git@github.com:xmos/lib_dsp.git"
        sh "git clone git@github.com:xmos/lib_logging.git"
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
          sh 'echo "will build stuff here"'
          // sh 'cd tests/test_callback && make'
          // if you want to build once and distribute to multiple later stages
          // use "stash/unstash"
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
                      sh 'python -m pytest test_mels.py --junitxml=pytest_result.xml -s'
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
