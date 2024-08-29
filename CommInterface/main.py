# -*- coding: utf-8 -*-
"""
Created on Sat Apr 20 16:45:21 2024

@author: mingqiang
"""

from GUI_main import RootGUI, ComGui
from Serial_Com_Ctrl import SerialCtrl
from Data_Com_Ctrl import DataMaster

MySerial = SerialCtrl()
Mydata = DataMaster()
Rootmain = RootGUI(MySerial, Mydata)
Commain = ComGui(Rootmain.root, MySerial, Mydata)


Rootmain.root.mainloop()