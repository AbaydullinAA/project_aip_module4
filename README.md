# Линейная аппроксимация и построение графика из CSV

Консольное приложение на C++17, которое считывает CSV-файл, выполняет линейную аппроксимацию (GSL) и строит график с помощью sciplot (v1.2.1). Пользователь передаёт имя файла, число пропускаемых строк, индексы столбцов для X и Y, а также количество точек для линии.

## Зависимости и сборка

```bash
# macOS (Homebrew)
brew install gsl gnuplot
git clone --branch v1.2.1 --depth 1 https://github.com/sciplot/sciplot.git
make
./build/fitting icecream.csv 1 0 1 50
