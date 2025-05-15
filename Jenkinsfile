pipeline {
    agent any

    environment {
        GIT_CREDENTIALS = credentials('GIT-PAT')
    }

    stages {
        stage('Checkout Firmware Code') {
            steps {
                git url: 'git@github.com:anisjellazi/tpe-firmware-sim.git',
                    branch: 'main',
                    credentialsId: 'GIT-SSH'
            }
        }

        stage('Build Firmware') {
            steps {
                sh 'make clean && make'
            }
        }

        stage('Static Analysis - cppcheck') {
            steps {
                sh 'cppcheck --enable=all --inconclusive firmware_tpe_simulator.c 2> cppcheck-report.xml'
            }
        }

        stage('Static Analysis - SonarQube') {
            steps {
                withCredentials([string(
                    credentialsId: 'devsecops_sonar_token',
                    variable: 'SONAR_TOKEN'
                )]) {
                    sh '''
                        sonar-scanner \
                          -Dsonar.projectKey=TPE-Firmware \
                          -Dsonar.sources=. \
                          -Dsonar.language=c \
                          -Dsonar.host.url=http://192.168.0.223:9000 \
                          -Dsonar.login=$SONAR_TOKEN
                    '''
                }
            }
        }

        stage('Filesystem Scan - Trivy') {
            steps {
                sh 'trivy fs --format json -o trivy-fs-report.json .'
            }
        }

        stage('Build Docker Image') {
            steps {
                sh 'docker build -t chonosus2001/tpe-firmware:latest .'
            }
        }

        stage('Scan Docker Image - Trivy') {
            steps {
                script {
                    catchError(buildResult: 'SUCCESS', stageResult: 'FAILURE') {
                        def exitCode = sh(
                            script: '''
                                mkdir -p target
                                trivy image --format json \
                                    --output target/trivy_report.json \
                                    --exit-code 1 \
                                    --severity HIGH,CRITICAL \
                                    --timeout 30m                                     chonosus2001/tpe-firmware:latest || true
                            ''',
                            returnStatus: true
                        )
                        if (exitCode == 1) {
                            echo "Trivy found vulnerabilities, marking stage as FAILED but continuing..."
                        } else if (exitCode != 0) {
                            error("Trivy encountered an unexpected error!")
                        }
                    }
                }
            }
        }

        stage('Push Docker Image') {
            steps {
                script {
                    withDockerRegistry(credentialsId: 'docker-cred', toolName: 'docker') {
                        sh 'docker push chonosus2001/tpe-firmware:latest'
                    }
                }
            }
        }

        stage('Deploy Firmware via Ansible') {
            steps {
                withCredentials([sshUserPrivateKey(
                    credentialsId: 'devops_ansible_ssh_key',
                    keyFileVariable: 'SSH_KEY',
                    usernameVariable: 'SSH_USER'
                )]) {
                    sh '''
                        chmod 600 $SSH_KEY
                        ssh -o StrictHostKeyChecking=no -p 2221 \
                            -i $SSH_KEY -T $SSH_USER@192.168.0.109 << 'EOF'
                        cd /home/chonosus/ansible-projects/devops-ansible-deployment
                        ansible-playbook -i 00_inventory.yml playbooks/deploy_tpe_firmware.yml \
                            --ask-vault-pass <<< "nejib"
                        exit
                        EOF
                    '''
                }
            }
        }

        stage('Archive Reports') {
            steps {
                archiveArtifacts artifacts: '''
                    target/trivy_report.json,
                    trivy-fs-report.json,
                    cppcheck-report.xml
                ''', allowEmptyArchive: true
            }
        }
    }

    post {
        always {
            script {
                def gdriveFolder = "TPE-Firmware"
                def reportFiles = [
                    "target/trivy_report.json",
                    "trivy-fs-report.json",
                    "cppcheck-report.xml"
                ]
                reportFiles.each { file ->
                    if (fileExists(file)) {
                        sh "rclone copy ${file} gdrive:${gdriveFolder} --progress"
                    }
                }
            }
        }
    }
}
