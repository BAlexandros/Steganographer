## Steganographer

#### Introduction 

_Steganography_ is the practice of concealing messages or information within other
non-secret text or data.
The necessity to conceal information can be traced back millenia, and has played
an important part throughout history.
One way to hide a message is to use some method of encryption, effectively
rendering the information useless to a person who wouldn't know how to reverse
the process. This method (called cryptography), however, usually results in 
unintelligible information which makes it obvious that a secret is hidden within 
the sequence present. 

The problem can be avoided by using another way to hide information: hiding it
in plain sight. 
This approach is greatly facilitated by the usage of computers. As all
information consists of streams of bytes, it is possible to alter the least
significant bit of each one in a way that they store a hidden message.
For example, altering the least significant bit in a pixel of an image file
wouldn't not lead to difference noticeable to the human eye, but would allow to
store one binary bit of hidden information.


#### Credits:
The convenient methods to process images were provided by Sean Barrett's
single file libraries: https://github.com/nothings/stb


