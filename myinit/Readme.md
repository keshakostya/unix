# Задача 3 - система инициализации

## Загрузка и сборка
```bash
$ git clone https://github.com/keshakostya/unix.git
$ cd unix/
$ git checkout feature/myinit
$ cd myinit
$ make
```

После выполнения команды `make` исполняемый файл будет лежать в `./build` по имени `myinit`

## Запуск тестового скрипта
```
$ ./runme.sh
$ cat ./build/result.txt
```

Скрипт систему инициализации в отдельном процессе.

Ситема инициализации запускает 3 `sleep` с разными значениями. Перезапускает самый быстрый из них.

Потом олучает сигнал `SIGHUP` и перезапускается с другой конфигурацией.

В `./build/result.txt` будет список всех запущенных процессов с логами.

Пример
```
Started three sleeps:
keshako+  861009  861005  0 21:50 pts/0    00:00:00 /usr/bin/sleep 5
keshako+  861008  861005  0 21:50 pts/0    00:00:00 /usr/bin/sleep 10
keshako+  861007  861005  0 21:50 pts/0    00:00:00 /usr/bin/sleep 15

Started one sleep after SIGHUP:
keshako+  861030  861005  0 21:50 pts/0    00:00:00 /usr/bin/sleep 7

Logs:
Started child [/usr/bin/sleep] with pid 861007
Started child [/usr/bin/sleep] with pid 861008
Started child [/usr/bin/sleep] with pid 861009
Child [/usr/bin/sleep] with pid 861009 exited with status 0. Restarting task
Started child [/usr/bin/sleep] with pid 861027
Caught SIGHUP (1). Restarting...
Started child [/usr/bin/sleep] with pid 861030
```