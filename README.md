# taxi_compare

Template of a C++ service that uses [userver framework](https://github.com/userver-framework/userver).


## Download and Build

To create your own userver-based service follow the following steps:

1. Press the "Use this template button" at the top right of this GitHub page
2. Clone the service `git clone your-service-repo && cd your-service-repo && git submodule update --init`
3. Give a proper name to your service and replace all the occurrences of "taxi_compare" string with that name
4. Feel free to tweak, adjust or fully rewrite the source code of your service.


## Makefile

`PRESET` is either `debug`, `release`, or if you've added custom presets in `CMakeUserPresets.json`, it
can also be `debug-custom`, `release-custom`.

* `make cmake-PRESET` - run cmake configure, update cmake options and source file lists
* `make build-PRESET` - build the service
* `make test-PRESET` - build the service and run all tests
* `make start-PRESET` - build the service, start it in testsuite environment and leave it running
* `make install-PRESET` - build the service and install it in directory set in environment `PREFIX`
* `make` or `make all` - build and run all tests in `debug` and `release` modes
* `make format` - reformat all C++ and Python sources
* `make dist-clean` - clean build files and cmake cache
* `make docker-COMMAND` - run `make COMMAND` in docker environment
* `make docker-clean-data` - stop docker containers


## Как запустить проект

1) Необходимо склонировать проект через `git clone https://github.com/Yozheeqq/CompareTaxi.git`
2) Перейти в директорию CompareTaxi
3) Запустить команду `make build-release`. Она установит все необходимые файлы
4) Далее необоходимо установить кафку.
```sh
wget -c https://dlcdn.apache.org/kafka/3.9.0/kafka_2.13-3.9.0.tgz
tar -xzf kafka_2.13-3.9.0.tgz
mv kafka_2.13-3.9.0.tgz /etc/kafka
```
5) Для запуска команд кафки из консоли необходимо установить java.
```sh
sudo apt update && sudo apt install -y openjdk-11-jre
java -version
# readlink -f $(which java)
# /usr/lib/jvm/java-11-openjdk-amd64/bin/java
echo 'export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64' >> ~/.bashrc
source ~/.bashrc
```
5) Потом надо создать файл secdist.json. Внутри примерно так:
```json
{
    "kafka_settings": {
        "kafka-producer": {
            "bootstrap.servers": "localhost:9092",
            "username": "",
            "password": "",
            "security.protocol": "PLAINTEXT"
        }
    }
}
```
6) Путь до этого файла указать в default-secdist-provider:config
7) Запустить сервис `cd ~/CompareTaxi & ./build-release/taxi_compare -c configs/static_config.yaml --config_vars configs/config_vars.yaml`
8) Можно проверить и отправить курл запрос:
```
curl -X POST -i --data '{"topic": "test-topic", "key": "key", "payload": "my message"}' localhost:8080/s
et-price-info
```
9) Потом можно вручную прочитать топик
```
./etc/kafka/bin/kafka-console-consumer.sh --bootstrap-server localhost:9092 --topic test-topic --from-beginning
```

## License

The original template is distributed under the [Apache-2.0 License](https://github.com/userver-framework/userver/blob/develop/LICENSE)
and [CLA](https://github.com/userver-framework/userver/blob/develop/CONTRIBUTING.md). Services based on the template may change
the license and CLA.
