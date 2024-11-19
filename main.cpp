#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QRegularExpression>
#include <QPushButton>
#include <QComboBox>
#include <QTest>
#include <stdexcept>

class Car {
public:
    Car(int id, const QString &engineType, const QString &year, const QString &brand, const QString &duration, bool engineOn, bool lightsOn, bool alarmOn)
        : id(id), engineType(engineType), year(year), brand(brand), duration(duration), engineOn(engineOn), lightsOn(lightsOn), alarmOn(alarmOn) {}

    int getId() const { return id; }
    QString getEngineType() const { return engineType; }
    QString getYear() const { return year; }
    QString getBrand() const { return brand; }
    QString getDuration() const { return duration; }
    bool isEngineOn() const { return engineOn; }
    bool isLightsOn() const { return lightsOn; }
    bool isAlarmOn() const { return alarmOn; }

    void setEngineType(const QString &value) { engineType = value; }
    void setYear(const QString &value) { year = value; }
    void setBrand(const QString &value) { brand = value; }
    void setDuration(const QString &value) { duration = value; }
    void setEngineOn(bool value) { engineOn = value; }
    void setLightsOn(bool value) { lightsOn = value; }
    void setAlarmOn(bool value) { alarmOn = value; }

private:
    int id;
    QString engineType;
    QString year;
    QString brand;
    QString duration;
    bool engineOn;
    bool lightsOn;
    bool alarmOn;
};

class CarManager : public QWidget {
    Q_OBJECT

public:
    CarManager(QWidget *parent = nullptr) : QWidget(parent), currentId(1) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        tableWidget = new QTableWidget(this);
        tableWidget->setColumnCount(8);
        tableWidget->setHorizontalHeaderLabels({"ID", "Тип двигателя", "Год производства", "Марка", "Продолжительность эксплуатации", "Двигатель", "Фары", "Сигнализация"});

        tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        tableWidget->setMinimumWidth(800);
        tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

        tableWidget->setStyleSheet("QTableWidget { border: 1px solid #ccc; }"
                                   "QHeaderView::section { background-color: white; border: 1px solid #ccc; color: black; font-weight: bold; }"
                                   "QTableWidget::item { padding: 5px; text-align: center; color: white; background-color: #444; }");

        QPushButton *actionButton = new QPushButton("Выбрать действие", this);
        QPushButton *resetButton = new QPushButton("Сбросить поиск", this);

        connect(actionButton, &QPushButton::clicked, this, &CarManager::chooseAction);
        connect(resetButton, &QPushButton::clicked, this, &CarManager::resetSearch);

        layout->addWidget(tableWidget);
        layout->addWidget(actionButton);
        layout->addWidget(resetButton);

        setLayout(layout);
        setWindowTitle("Управление машинами");
    }

    // Методы для тестирования
    void addCar(const QString &engineType, const QString &year, const QString &brand, const QString &duration, bool engineOn, bool lightsOn, bool alarmOn) {
        cars.append(Car(currentId++, engineType, year, brand, duration, engineOn, lightsOn, alarmOn));
        displayCars();
    }

    void removeLastCar() {
        if (cars.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Нет машин для удаления.");
            return;
        }
        cars.removeLast();
        currentId--;  // Уменьшаем текущий ID
        resetSearch();
    }

    int getCarCount() const { return cars.size(); }

    Car searchCarById(int id) const {
        for (const Car &car : cars) {
            if (car.getId() == id) {
                return car;
            }
        }
        return Car(-1, "", "", "", "", false, false, false); // Возвращаем пустой Car, если не найден
    }

    void modifyCarData(int id, const QString &engineType, const QString &year, const QString &brand, const QString &duration, bool engineOn, bool lightsOn, bool alarmOn) {
        for (Car &car : cars) {
            if (car.getId() == id) {
                car.setEngineType(engineType);
                car.setYear(year);
                car.setBrand(brand);
                car.setDuration(duration);
                car.setEngineOn(engineOn);
                car.setLightsOn(lightsOn);
                car.setAlarmOn(alarmOn);
                resetSearch();
                return; // Выход после изменения
            }
        }
        QMessageBox::warning(this, "Ошибка", "Машина с таким ID не найдена.");
    }

private slots:
    void chooseAction() {
        QStringList actions = {"Добавить машину", "Удалить последнюю машину", "Удалить машину по ID", "Поиск машины", "Поиск по ID", "Изменить данные у машины"};
        QString action = QInputDialog::getItem(this, "Выберите действие", "Действие:", actions, 0, false);

        if (action.isEmpty()) return;

        if (action == "Добавить машину") {
            addCar(); // Упрощаем добавление автомобиля
        } else if (action == "Удалить последнюю машину") {
            removeLastCar();
        } else if (action == "Удалить машину по ID") {
            removeCarById();
        } else if (action == "Поиск машины") {
            searchCar();
        } else if (action == "Поиск по ID") {
            searchCarById();
        } else if (action == "Изменить данные у машины") {
            modifyCarData();
        }
    }

    void addCar() {
        QString engineType = QInputDialog::getText(this, "Добавить машину", "Тип двигателя:");
        if (!validateEngineType(engineType)) return;

        QString year = QInputDialog::getText(this, "Добавить машину", "Год производства:");
        if (!validateYear(year)) return;

        QString brand = QInputDialog::getText(this, "Добавить машину", "Марка:");
        if (!validateBrand(brand)) return;

        QString duration = QInputDialog::getText(this, "Добавить машину", "Продолжительность эксплуатации:");
        if (!validateDuration(duration)) return;

        bool engineOn = QMessageBox::question(this, "Добавить машину", "Двигатель включен?") == QMessageBox::Yes;
        bool lightsOn = QMessageBox::question(this, "Добавить машину", "Фары включены?") == QMessageBox::Yes;
        bool alarmOn = QMessageBox::question(this, "Добавить машину", "Сигнализация включена?") == QMessageBox::Yes;

        addCar(engineType, year, brand, duration, engineOn, lightsOn, alarmOn);
    }

    void removeCarById() {
        bool ok;
        int id = QInputDialog::getInt(this, "Удалить машину по ID", "Введите ID:", 1, 1, INT_MAX, 1, &ok);
        if (ok) {
            auto it = std::remove_if(cars.begin(), cars.end(), [id](const Car &car) { return car.getId() == id; });
            if (it != cars.end()) {
                cars.erase(it, cars.end());
                resetSearch();
            } else {
                QMessageBox::warning(this, "Ошибка", "Машина с таким ID не найдена.");
            }
        }
    }

    void searchCar() {
        QString characteristic = QInputDialog::getItem(this, "Поиск по характеристикам", "Выберите характеристику:", {"Тип двигателя", "Год производства", "Марка", "Продолжительность эксплуатации", "Двигатель", "Фары", "Сигнализация"}, 0, false);
        if (characteristic.isEmpty()) return;

        QString value = QInputDialog::getText(this, "Поиск по характеристикам", "Введите значение для поиска:");
        if (value.isEmpty()) return;

        tableWidget->clearContents();
        tableWidget->setRowCount(0);

        for (const Car &car : cars) {
            bool match = false;

            if (characteristic == "Тип двигателя" && car.getEngineType() == value) {
                match = true;
            } else if (characteristic == "Год производства" && car.getYear() == value) {
                match = true;
            } else if (characteristic == "Марка" && car.getBrand() == value) {
                match = true;
            } else if (characteristic == "Продолжительность эксплуатации" && car.getDuration() == value) {
                match = true;
            } else if (characteristic == "Двигатель" && (car.isEngineOn() ? "Да" : "Нет") == value) {
                match = true;
            } else if (characteristic == "Фары" && (car.isLightsOn() ? "Да" : "Нет") == value) {
                match = true;
            } else if (characteristic == "Сигнализация" && (car.isAlarmOn() ? "Да" : "Нет") == value) {
                match = true;
            }

            if (match) {
                int rowCount = tableWidget->rowCount();
                tableWidget->insertRow(rowCount);
                tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(car.getId())));
                tableWidget->setItem(rowCount, 1, new QTableWidgetItem(car.getEngineType()));
                tableWidget->setItem(rowCount, 2, new QTableWidgetItem(car.getYear()));
                tableWidget->setItem(rowCount, 3, new QTableWidgetItem(car.getBrand()));
                tableWidget->setItem(rowCount, 4, new QTableWidgetItem(car.getDuration()));
                tableWidget->setItem(rowCount, 5, new QTableWidgetItem(car.isEngineOn() ? "Да" : "Нет"));
                tableWidget->setItem(rowCount, 6, new QTableWidgetItem(car.isLightsOn() ? "Да" : "Нет"));
                tableWidget->setItem(rowCount, 7, new QTableWidgetItem(car.isAlarmOn() ? "Да" : "Нет"));
            }
        }
    }

    void searchCarById() {
        bool ok;
        int id = QInputDialog::getInt(this, "Поиск машины по ID", "Введите ID:", 1, 1, INT_MAX, 1, &ok);
        if (ok) {
            tableWidget->clearContents();
            tableWidget->setRowCount(0);

            for (const Car &car : cars) {
                if (car.getId() == id) {
                    int rowCount = tableWidget->rowCount();
                    tableWidget->insertRow(rowCount);
                    tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(car.getId())));
                    tableWidget->setItem(rowCount, 1, new QTableWidgetItem(car.getEngineType()));
                    tableWidget->setItem(rowCount, 2, new QTableWidgetItem(car.getYear()));
                    tableWidget->setItem(rowCount, 3, new QTableWidgetItem(car.getBrand()));
                    tableWidget->setItem(rowCount, 4, new QTableWidgetItem(car.getDuration()));
                    tableWidget->setItem(rowCount, 5, new QTableWidgetItem(car.isEngineOn() ? "Да" : "Нет"));
                    tableWidget->setItem(rowCount, 6, new QTableWidgetItem(car.isLightsOn() ? "Да" : "Нет"));
                    tableWidget->setItem(rowCount, 7, new QTableWidgetItem(car.isAlarmOn() ? "Да" : "Нет"));
                    break;
                }
            }
        }
    }

    void modifyCarData() {
        bool ok;
        int id = QInputDialog::getInt(this, "Изменить данные у машины", "Введите ID машины:", 1, 1, INT_MAX, 1, &ok);
        if (ok) {
            for (Car &car : cars) {
                if (car.getId() == id) {
                    QString engineType = QInputDialog::getText(this, "Изменить данные", "Тип двигателя:", QLineEdit::Normal, car.getEngineType());
                    if (validateEngineType(engineType)) car.setEngineType(engineType);

                    // Остальные валидации
                    QString year = QInputDialog::getText(this, "Изменить данные", "Год производства:", QLineEdit::Normal, car.getYear());
                    if (validateYear(year)) car.setYear(year);

                    QString brand = QInputDialog::getText(this, "Изменить данные", "Марка:", QLineEdit::Normal, car.getBrand());
                    if (validateBrand(brand)) car.setBrand(brand);

                    QString duration = QInputDialog::getText(this, "Изменить данные", "Продолжительность эксплуатации:", QLineEdit::Normal, car.getDuration());
                    if (validateDuration(duration)) car.setDuration(duration);

                    bool engineOn = QMessageBox::question(this, "Изменить данные", "Двигатель включен?") == QMessageBox::Yes;
                    car.setEngineOn(engineOn);

                    bool lightsOn = QMessageBox::question(this, "Изменить данные", "Фары включены?") == QMessageBox::Yes;
                    car.setLightsOn(lightsOn);

                    bool alarmOn = QMessageBox::question(this, "Изменить данные", "Сигнализация включена?") == QMessageBox::Yes;
                    car.setAlarmOn(alarmOn);

                    resetSearch();
                    return;
                }
            }
            QMessageBox::warning(this, "Ошибка", "Машина с таким ID не найдена.");
        }
    }

    void resetSearch() {
        tableWidget->clearContents();
        tableWidget->setRowCount(0);
        displayCars();
    }

    void displayCars() {
        for (const Car &car : cars) {
            int rowCount = tableWidget->rowCount();
            tableWidget->insertRow(rowCount);
            tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(car.getId())));
            tableWidget->setItem(rowCount, 1, new QTableWidgetItem(car.getEngineType()));
            tableWidget->setItem(rowCount, 2, new QTableWidgetItem(car.getYear()));
            tableWidget->setItem(rowCount, 3, new QTableWidgetItem(car.getBrand()));
            tableWidget->setItem(rowCount, 4, new QTableWidgetItem(car.getDuration()));
            tableWidget->setItem(rowCount, 5, new QTableWidgetItem(car.isEngineOn() ? "Да" : "Нет"));
            tableWidget->setItem(rowCount, 6, new QTableWidgetItem(car.isLightsOn() ? "Да" : "Нет"));
            tableWidget->setItem(rowCount, 7, new QTableWidgetItem(car.isAlarmOn() ? "Да" : "Нет"));
        }
    }

    bool validateEngineType(const QString &engineType) {
        if (engineType.length() < 2) {
            QMessageBox::warning(this, "Ошибка", "Тип двигателя должен содержать не менее 2 символов.");
            return false;
        }

        if (engineType.contains(QRegularExpression("\\d"))) {
            QMessageBox::warning(this, "Ошибка", "Тип двигателя не может содержать цифры.");
            return false;
        }

        return true;
    }


    bool validateYear(const QString &year) {
        if (year.isEmpty() || !QRegularExpression("\\d{4}").match(year).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Год производства должен состоять из 4 цифр.");
            return false;
        }
        return true;
    }

    bool validateBrand(const QString &brand) {
        if (brand.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Марка не может быть пустой.");
            return false;
        }
        return true;
    }

    bool validateDuration(const QString &duration) {
        if (duration.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Продолжительность эксплуатации не может быть пустой.");
            return false;
        }
        return true;
    }

private:
    QTableWidget *tableWidget;
    QVector<Car> cars;
    int currentId;
};

class TestCarManager : public QObject {
    Q_OBJECT

private slots:
    void testAddCar() {
        CarManager carManager;
        carManager.addCar("Бензин", "2020", "Toyota", "4 года", true, false, false);
        QVERIFY(carManager.getCarCount() == 1);
    }

    void testRemoveCar() {
        CarManager carManager;
        carManager.addCar("Электрический", "2017", "Tesla", "8 лет", false, false, false);
        carManager.removeLastCar();
        QVERIFY(carManager.getCarCount() == 0);
    }

    void testSearchCarById() {
        CarManager carManager;
        carManager.addCar("Дизель", "2005", "Nissan", "19 лет", true, false, true);
        QCOMPARE(carManager.searchCarById(1).getBrand(), QString("Nissan"));
    }

    void testModifyCarData() {
        CarManager carManager;
        carManager.addCar("Электрический", "2022", "Xiaomi", "2 года", true, false, false);
        carManager.modifyCarData(1, "Дизель", "2019", "Honda", "3 года", false, true, true);
        QCOMPARE(carManager.searchCarById(1).getBrand(), QString("Honda"));
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    CarManager carManager;
    carManager.show();

    QTest::qExec(new TestCarManager, argc, argv);

    return a.exec();
}

#include "main.moc"
