# This is a sample Python script.
import kivy
import smtplib
import ssl
import os
from kivy.core.text import LabelBase
from kivy.resources import resource_add_path, resource_find
from email.message import EmailMessage

kivy.require('2.1.0')

from functools import partial
from kivy.app import App
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button


class MainLayout(GridLayout):
    def __init__(self, **kwargs):
        super(MainLayout, self).__init__(**kwargs)
        self.cols = 4
        self.showString = ''
        self.menuLists = []
        self.MyButtonList = [
            Button(text='炖烧排骨', state='normal', font_size=12, on_press=partial(self.press_button, "炖烧排骨")),
            Button(text='水煮肉片', state='normal', font_size=12, on_press=partial(self.press_button, "水煮肉片")),
            Button(text='香辣红烧肉', state='normal', font_size=12, on_press=partial(self.press_button, "香辣红烧肉")),
            Button(text='蜜汁鸡翅', state='normal', font_size=12, on_press=partial(self.press_button, "蜜汁鸡翅")),
            Button(text='番茄炒蛋', state='normal', font_size=12, on_press=partial(self.press_button, "番茄炒蛋")),
            Button(text='酸辣土豆丝', state='normal', font_size=12, on_press=partial(self.press_button, "酸辣土豆丝")),
            Button(text='番茄鸡蛋汤', state='normal', font_size=12, on_press=partial(self.press_button, "番茄鸡蛋汤"))
        ]
        for item in self.MyButtonList:
            self.add_widget(item)

        self.menuBtn = Button(text='已点...', on_press=partial(self.press_menuBtn))
        self.add_widget(self.menuBtn)
        self.sendBtn = Button(text="发送", on_press=partial(self.press_sendBtn))
        self.add_widget(self.sendBtn)
        self.showText = TextInput(text='已点:\n', font_size=12)
        self.showText.readonly = True
        self.add_widget(self.showText)
        self.addText = TextInput(text='添加:\n', font_size=12)
        self.add_widget(self.addText)

    def press_sendBtn(self, *arg):
        self.send_email()

    def press_menuBtn(self, *arg):
        self.showText.text = self.showString

    def press_button(self, *arg):
        # print(arg)
        self.updateMenu(str(arg[0]))

    def updateMenu(self, string):
        self.showString = '已点:\n'
        if string in self.menuLists:
            index = self.menuLists.index(string)
            self.menuLists.pop(index)
            for item in self.menuLists:
                self.showString += str(item) + '\n'
        else:
            self.menuLists.append(string)
            for item in self.menuLists:
                self.showString += str(item) + '\n'

        # print(self.showString)

    def send_email(self):
        self.showText.text = ''
        key = "kbrdgdlrjhhsieeb"
        EMAIL_ADDRESS = "1462134792@qq.com"
        EMAIL_PASSWORD = key
        smtp = smtplib.SMTP("smtp.qq.com", 25)
        context = ssl.create_default_context()
        sender = EMAIL_ADDRESS
        receiver = EMAIL_ADDRESS
        subject = "菜单"
        body = self.showString + '\n' + self.addText.text
        msg = EmailMessage()
        msg['subject'] = subject
        msg['from'] = sender
        msg['to'] = receiver
        msg.set_content(body)
        with smtplib.SMTP_SSL("smtp.qq.com", 465, context=context) as smtp:
            smtp.login(EMAIL_ADDRESS, EMAIL_PASSWORD)
            smtp.send_message(msg)
        self.showText.text = '发送成功'


class ButtonApp(App):
    def build(self):
        resource_add_path(os.path.abspath('.'))
        LabelBase.register('Roboto', 'msyhl.ttc')
        return MainLayout()


if __name__ == '__main__':
    ButtonApp().run()
