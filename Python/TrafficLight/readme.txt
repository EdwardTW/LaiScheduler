Jetson Nano or Raspberry Pi

sudo apt install python-pip
pip install pygame
pip install enum
or
sudo apt-get install python-pygame

downnload my  source code > /home/edward/Downloads/PythonProject
cp /home/edward/Downloads/PythonProject/LaiScheduler.py /usr/lib/python2.7/dist-packages
(pi > /usr/lib/python2.7)(Mac > /Library/Python/2.7/site-packages)
cd /home/edward/Downloads/PythonProject
python TrafficLight.py

python -c "import pygame as _; print(_.__path__)"
in python
print dir(LaiScheduler)

sudo apt-get install git-all
git clone https://github.com/NVIDIA/jetson-gpio.git
cd jetson-gpio
sudo python3 setup.py install

$wget --no-check-certificate https://pypi.python.org/packages/source/s/setuptools/setuptools-12.0.3.tar.gz#md5=f07e4b0f4c1c9368fcd980d888b29a65
$ tar -zxvf setuptools-12.0.3.tar.gz
$ cd setuptools=12.0.3
$ python setup.py install

