OS
==
#OS Class Project 14_15

#Summary

So, we got to develop a Server, that server will be ready to receive at any moment a request from a plane Pilot.

Those Pilots are going to ask the Server for some informations about the weather (like shitstorms, etc).

The Server will receive these request on a unique pipe, and send answers to the Pilots on individuals pipes named after their uniques pid.

Our Server will take his weather informations from a file generated by a Meteo generator, the deal with this point is to prevent any concurrent access between the reader Server and the writer Meteo Generator.

##Files we gotta code

- Pilot.c
- Server.c
- Meteogen.c
- Structure.h (At least for ATIS messages structure)

##Summarized critical constraints

- Prevent concurrential access between Server and Meteo on the meteo file
- Make our little boys sure that they read the correct amount of bytes.
- ...

##To give on the d-date

- Word document report :
  - Introduction
  - Analysis (Plan of the app)
  - Explanations on specifics technical items
  - Conclusion (Problems and difficulties encountered, solutions, improvements)    
- Printed Code
