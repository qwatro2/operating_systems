# ОС, ДЗ5
## Власов Николай, БПИ229

### сделал на 10

#### как можно запускать
\$ python3 builder.py&emsp;&emsp;&emsp;// компилирует файлы и выводит подсказку \
\$ tmux&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;// утилита для разделения терминала \
\$ tmux split-window -h&emsp;&emsp;// деление терминала на две части вертикальной линией, для навигации я использую Ctrl+B, затем стрелка вправо/влево \
\$ ./sender&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;// запустить в одной половине sender \
\$ ./receiver&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;// запустить в другой половине receiver \
\
Для выхода из tmux можно в каждом окне нажать Ctrl+D.

#### Файл builder.py
Python-скрипт для компиляции и вывода подсказки

#### Файл sender.c
Код на C, реализующий логику sender-а

#### Файл receiver.c
Код на C, реализующий логику receiver-а
