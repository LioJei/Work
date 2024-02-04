import sys
from PyQt5.QtWidgets import QPushButton, QApplication, QWidget, QMainWindow
from MainWindow import *


class WinForm(QWidget):
    def __init__(self, parent=None):
        super(WinForm, self).__init__(parent)
        self.setGeometry(300, 300, 350, 350)
        self.setWindowTitle("Clicked the button to close this window")
        quit_btn = QPushButton("close", self)
        quit_btn.setGeometry(10, 10, 60, 35)
        quit_btn.setStyleSheet("background-color: red")
        quit_btn.clicked.connect(self.close)


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.setupUi(self)  # 设置界面
        self.pushButton.clicked.connect(self.sayHi)  # 绑定点击信号和槽函数

    def sayHi(self):  # click对应的槽函数
        print("hi")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = MainWindow()
    win.show()
    sys.exit(app.exec_())
