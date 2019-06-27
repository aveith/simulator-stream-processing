********************************************************************************
********************************************************************************
            DATA STREAM PROCESSIN SIMULATOR ATOP OMNET++ 5.*
********************************************************************************
********************************************************************************
SUMMARY
    1 - INSTALLING AND SETUP
    2 - ENVIRONMENT DESCRIPTION
    3 - RUNNING SIMULATIONS
   
   
   
1 - INSTALLING AND SETUP

    1.1 - PREREQUISITES
        - OMNET++ 5.*
        - Python
        
    1.2 - BUILD PROJECT
        In the root folder:
            - make clean
            - make

2 - ENVIRONMENT DESCRIPTION

    1.1 - OVERVIEW
        The simulator requires as input two files:
            a) One file describing the Application (.dot or .xml). By passing 
            the .xml file, the document must have tags for operators and 
            streams.
            b) Network (.xml).
        
        
        
        The DSP discrester simulators comprises two main phases:
            a) DSP Configuration (static deployment); and
            c) Application Reconfiguration (dynamic deployment).      
