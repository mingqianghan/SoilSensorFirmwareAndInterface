from AppRoot import AppRoot
from Serial_Com_Ctrl import SerialCtrl
from Data_Com_Ctrl import DataMaster

# API Key to call openweathermap to get weather summary
api_key = '249c9903b0a3e6f5692438b7016a7db1'

serial = SerialCtrl()
data = DataMaster()

App = AppRoot(api_key, serial, data)
App.root.mainloop()

