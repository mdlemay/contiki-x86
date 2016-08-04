HTTP-controlled MIDI alert bell connected via Ethernet
======================================================

Introduction
------------
Generate a configurable alert bell by sending an HTTP GET request to an
Ethernet-connected Intel Galileo Arduino board with a MIDI synthesizer
shield.

Hardware setup
--------------
1.  Setup the Galileo as described in platform/galileo/README.md.
2.  Install a Modern Device Fluxamasynth Shield v.3 [1].
3.  Insert a 1K-ohm resistor between IO1 to IO4 to connect the TX pin of UART0
    to the serial input of the MIDI synthesizer.
4.  Connect an audio output device to the synthesizer.
5.  Connect the Galileo to an Ethernet network.

Building software
-----------------
1.  Follow the instructions in platform/galileo/README.md to prepare the build
    environment.
2.  Navigate to the directory containing this README in the prepared build
    environment.
3.  If desired, configure a static IP for the Galileo by editing the
    definitions in platform/galileo/net/eth-conf.c. Remember to also reserve
    that static IP in the network to which the Galileo will be connected.
4.  Run the following command to build the Contiki OS image:
    `make BUILD_RELEASE=1`
5.  Copy http-synth.galileo.efi to a microSD card as described in
    platform/galileo/README.md for hello-world.galileo.efi.

Usage
-----
1.  Boot the Contiki OS image that was built using the instructions above.
2.  The alert bell should play once during boot through the attached audio
    output device.
3.  Navigate to http://<IP address>/synth.shtml to play the alert bell again.
    It should play once for each HTTP GET request that is received.

Customization
-------------
1.  To select a different instrument, pitch, or velocity for the alert bell,
    edit the definitions near the top of http-synth.c.

Notes/cautions
--------------
1.  Anyone that can send HTTP requests to the Galileo can potentially cause it
    to play notes.
2.  Various services are available online that can send HTTP GET requests in
    response to configurable triggers. For example, I successfully tested
    IFTTT with my device [2].

References
----------
1.  https://moderndevice.com/product/fluxamasynth-shield/
2.  https://ifttt.com
