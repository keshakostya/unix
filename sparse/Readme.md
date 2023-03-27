# Задача 1 - sparse файл

## Загрузка и сборка
```bash
$ git clone https://github.com/keshakostya/unix.git
$ cd unix/sparse
$ git checkout feature/sparse
$ make
```

После выполнения команды `make` исполняемый файлы будет лежать в `./build` по имени `sparse`

## Запуск тестового скрипта
```
$ ./runme.sh
$ cat ./build/result.txt
```

Скрипт создает файл `fileA` и по разному с ним манипулирует. Конкретная информация будет лежать в ./build/result.txt