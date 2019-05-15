# iOS Demo Nabto 5 Demo App

This app demonstrates interacting with the Nabto5 client api from iOS.

When pressing the + button, a new connection is made. A stream is opened towards a stream echo Nabto embedded server and a few bytes are written to the stream. Once the echo response is received, the latency is written to a table.

Very rudimentary implementation, for instance all device parameters are just embedded in source code in the `nabtoDemoConnect` function in MasterViewController.mm. So make sure to change the parameters to match your device. The cli-demo matching the parameters embedded in the iOS app has the following commandline:

```
./test_device  -p pr-wrukmj9j -d de-jwmawg7z -k ug-device.pem --hostname nabto5.dev.nabto.net
```

Parameters are just shown for reference to compare with the iOS app source code, you will need to replace parameters with your own that are registered in the Nabto Cloud console with the fingerprint of your own public key.