# Nabto 5 Beta1 Release
This repo captures artifacts, documentation and issues for the Nabto5 Beta1 release.

This first beta provides early access to the Nabto5 Client and Embedded SDKs as well as a working basestation and the Nabto Cloud Console solution management application.

To get started, the following path is suggested (outlined in detail in subsequent sections):

1. obtain access to the Nabto Cloud Console
2. setup a solution in Nabto Cloud
3. build and run an embedded device demo
4. build and run a client demo app

# Limitations and Known Issues in the Beta1 Release

* Android support is not yet ready and has not been included in the Beta1 release, an upcoming patch release that includes Android support will follow shortly.
* API error code representations has not been fully decided. For now, just check for `NABTO_CLIENT_OK` and `NABTO_DEVICE_OK` when checking for success, detailed error codes when status is not OK cannot yet be fully trusted (but will be different than the OK codes in case of error).
* Log configuration has not been fully decided.
* Documentation is limited to annotation in header files.
* No association between console users and domains - all console users have access to all Nabto5 entities created in the system. So this means that you should not add any potentially business sensitive information as e.g. product names.


# Console Access

To use the Nabto Cloud console in its current incarnation (ie, without a signup feature), write to ug@nabto.com to get an account prepared. The account will be created on the development cloud system. Ideally, for confidientiality, credentials should be exchanged using secure communication, e.g. GPG encrypted mail or e.g. Signal IM. Given that the accounts will not be used for production, it is acceptable with some less strict exchange of credentials, though.

Access to the console requires the Google Authenticator app installed on a mobile device.

Note that all functionality in the Console application will be made available through a REST API for integration in your own device management solution.

## Products

Creating a new product in the console is the first step towards running a device and client. Just click "Products" in the menu on the left and then "New Product" and save.


## Devices

Devices must be added through the console to obtain a device id and register a public key fingerprint, in turn allowing the device to later attach (register with the Nabto basestation). In the product overview, a new device can be created by clicking the "Devices" link for a given product and then clicking "Create New Device".

In the "Fingerprint" field of the new device, enter the public key fingerprint for the keypair to use with the device in question. See the Embedded SDK section below on how to create this keypair and obtain the fingerprint.

## Client apps

Client apps need an api key. For a given product, multiple client apps can be created. Click the "Apps" link in the product overview and click the "New App" button.


# Embedded SDK and demos

## Creating keypair

```
openssl ecparam -genkey -name prime256v1 -out <keyfile>
```

# Client SDK and demos