## Установка conan
``` bash
pip3 install conan
conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default
```
## Сборка проекта из директории исходников
``` bash
conan install .
cmake . -DCMAKE_BUILD_TYPE=Release
make
```
