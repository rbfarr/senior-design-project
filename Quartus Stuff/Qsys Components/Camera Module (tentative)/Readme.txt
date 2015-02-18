This should be everything that is needed to load and instantiate the Camera's
Qsys Module. 

Top Level:
  CCD_avalon_interface

Constituents:
  CCD_Capture: Interfaces directly with GPIO pins
  I2C_CCD_Config: Interfaces with GPIO pins to provide commands to camera
    I2C_Controller
  RAW2RGB: Converts camera's pixel representation into RGB values
  Reset_Delay: Controls various reset parameters
