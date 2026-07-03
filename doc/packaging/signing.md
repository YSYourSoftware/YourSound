# Signing Libraries

You might notice that when users install your library, they get a warning telling them that *"the integrity of the
library could not be verified"*.
This happens because your library has not been signed, or the public key was not found.

Here's what you can do:

| Result                                                                       | Action                                                                                                                        |
|------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------|
| Change warning to *"The library has been signed by an unverified publisher"* | Sign the library (see below), then upload the public key to the [OpenPGP KeyServer](https://keys.openpgp.org).                |
| Remove warning                                                               | Sign up to the [partner program](https://sound.yoursoftware.org/partner), then upload the library through the partner portal. |

## Unverified Signing (self-signing)

To sign a library, you will need the [SArc tool executables](https://github.com/YSYourSoftware/SArc/releases/latest).
Then, on the `.yslib` library file, run `SArcEdit --pgp-sign `