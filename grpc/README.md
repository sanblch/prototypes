# Собрать пакет из папки conan
conan create . 1.34.1@gm/thirdparty -s build_type=Debug
# Собрать клиент/сервер на c++
cmake ${source} && make
