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
3) Запустить скрипт `sudo ./setup.sh`. Он установит все необходимые пакеты
4) Запустить команду `make build-release`. Она соберет весь проект
5) Потом надо создать файл secdist.json. Внутри примерно так:
```json
{
    "kafka_settings": {
        "kafka-producer": {
            "brokers": "localhost:9092",
            "username": "",
            "password": ""
        },
        "kafka-consumer-profile-updater": {
            "brokers": "localhost:9092",
            "username": "",
            "password": ""
        },
        "kafka-consumer-ml-updater": {
            "brokers": "localhost:9092",
            "username": "",
            "password": ""
        }
    }
}

```
6) Путь до этого файла указать в default-secdist-provider:config
7) Добавить файл с моделькой в models/model.onnx
8) Запустить http-server
```sh
cd ~/CompareTaxi
./build-release/http_server -c configs/http_server/static_config.yaml --config_vars configs/common/config_vars.yaml
```
9) Запустить ноды
```sh
./build-release/node -c configs/nodes/static_config.yaml --config_vars configs/common/config_vars.yaml
```
10) Можно проверить и отправить курл запрос:
```
curl -X GET -i --data @requests_sample/get_config.json localhost:8080/get-config
```
11) Потом можно пойти в логи и увидеть, что все хорошо

12) Для отправки метрик нужно перейти в папку `monitoring` и запустить в ней докер-файл
```
sudo docker-compose up -d
```
13) Далее нужно запустить кафку локально. Для этого надо перейти в папку `kafka` и запустить в ней докер файл
```
sudo docker-compose up -d
```

## License

The original template is distributed under the [Apache-2.0 License](https://github.com/userver-framework/userver/blob/develop/LICENSE)
and [CLA](https://github.com/userver-framework/userver/blob/develop/CONTRIBUTING.md). Services based on the template may change
the license and CLA.
