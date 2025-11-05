from AppRoot import AppRoot
from Serial_Com_Ctrl import SerialCtrl
from Data_Com_Ctrl import DataMaster

serial = SerialCtrl()
data = DataMaster()

App = AppRoot(serial, data)
App.root.mainloop()

