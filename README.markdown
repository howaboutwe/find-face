# Find Face

Simple command line tool wrapping OpenCV's Haar Cascade Classifier to perform
simple face detection on the command line.

## Example

    $ curl -s https://d267w4oc0y54w5.cloudfront.net/photos/903953/large_profile/1340672752.?1340672752 > brian.jpg
    $ ./find-face --cascade=haarcascade_frontalface_default.xml brian.jpg
    0.06 0.226667 0.463333 0.463333

Give it a whirl. You'll get a free picture of our attractive founder.

## Contributing

 * [Bug reports](https://github.com/oggy/find-face/issues)
 * [Source](https://github.com/oggy/find-face)
 * Patches: Fork on Github, send pull request.
   * Include tests where practical.
   * Leave the version alone, or bump it in a separate commit.

## Copyright

Copyright (c) George Ogata. See LICENSE for details.
