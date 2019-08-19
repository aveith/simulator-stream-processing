********************************************************************************
********************************************************************************
            DATA STREAM PROCESSING DISCRETE EVENT SIMULATOR ATOP OMNET++ 5.*
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
        The simulator requires two files as input:
            a) Aplication (.dot or .xml)
            b) Network (.xml).
        
                
        The DSP discrester simulators comprises two main phases:
            a) DSP Configuration (static deployment); and
            c) Application Reconfiguration (dynamic deployment).      
