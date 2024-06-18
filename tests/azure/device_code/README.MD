# Device Code

## Description

This scenario emulates an attack chain where an attacker leverages a device code phishing flow. The attacker begins by obtaining a device code, which is then used to craft a phishing email designed to trick the user into logging in using the provided code. Once the user authenticates, the attacker receives an access token. This access token is subsequently used to log into Microsoft Graph and perform malicious operations.

## Reference

- https://aadinternals.com/post/phishing/;
