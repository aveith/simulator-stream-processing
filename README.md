********************************************************************************
********************************************************************************
            DATA STREAM PROCESSIN SIMULATOR ATOP OMNET++ 5.4.1
********************************************************************************
********************************************************************************
SUMMARY
    1 - INSTALLING AND SETUP
    2 - ENVIRONMENT DESCRIPTION
    3 - RUNNING SIMULATIONS
   
   
   
1 - INSTALLING AND SETUP
    1.1 - PREREQUISITES
        - OMNET++ 5.4.1
        
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
        
        
        
        The simulated environment comprises three main phases:
            a) Configuration of the environment (static deployment);
            b) Collect statistics from the configuration deployment;
            c) Reconfiguration of the environment (dynamic deployment).
        
        The configuration phase comprises 