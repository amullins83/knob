#Knob - Using LibUSB to control a [Griffin PowerMate](http://store.griffintechnology.com/powermate)

This is a work in progress. The goal of this project is to demonstrate basic [libusb](http://libusb.org) functionality with the Griffin Powermate USB controller. Unfortunately, my development machine is a Mac, and OSX has kernel restrictions preventing HID devices from being used by applications in this way.

##Acknowledgements

The impetus for this project comes from attempting to debug the project by the same name in the [Chrome App Samples](https://github.com/GoogleChrome/chrome-app-samples) repo. Thanks to user [scheib](https://github.com/scheib) for bringing the issue to my attention!

##Use

Plug in a PowerMate. Run `make` on a linux command line. You should see text detailing each step in the process.

##Steps

1. Initialize libusb
2. Enumerate devices
3. Open the PowerMate device
4. Allocate and send an output request
5. Allocate and send an input request
6. Close the device
7. Deinitialize libusb
