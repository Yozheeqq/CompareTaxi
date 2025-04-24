# taxi_compare

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
