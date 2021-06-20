# MASB - POT - S Project 

## Introduction 

This project has consisted of the configuration and programming of a microcontroller to perform electrochemical measurements, such as chronoamperometry and cyclic voltammetry automatically by means of a potentiostat. It has been developed with the aim of achieving and consolidating all the knowledge seen and worked in the subject of Microcontrollers for Biomedical Applications and Systems. The authors of this work are Bruna Martín Català and Maria Mor Basart (see Figure 1 and Figure 2).



## Table of Contents 

1. Introduction 
   - Potentiostat 
   - Cyclic Voltammetry 
   - Chronoamperometry
   - Git and GitHub 
2. Objectives 
3. Project development 
   - *Workflow* of the cyclic voltammetry 
   - *Workflow* of the chronoamperometry 
4. Results 
5. Conclusions 
6. References 



### Potentiostat

This project is based on the programming of a potentiostat consisting of a front-end and a back-end based on the Evaluation Board (EVB) NUCLEO F401RE from STMicroelectronics. A potentiostat is an electronic device generally consisting of three electrodes and is used for electrochemical measurements and experiments. The system consists such that the working electrode must be maintained at a constant potential with respect to the reference electrode by adjusting the current at the auxiliary electrode [1]. Thus, the potentiostat must bias the electrochemical cell to a voltage Vcell and read the current Icell flowing through it. To set the Vcell voltage, we have the DAC MCP4725, whose I2C address is `1100000`. This model can generate an output voltage from 0 to 4 V, but a stage has been added to convert this signal to a bipolar signal from - 4 to 4 V to be able to bias to both positive and negative voltages [2]. The potentiostat used for this project can be seen represented in the following schematic (see Figure 3). 



### Cyclic Voltammetry 

*Cyclic Voltammetry* (CV) is an electrochemical measurement where information on the current of an analyte is obtained by changing the potential. These studies aim to study the reactivity in the half-cell of an analyte by controlling the potential of an electrode while measuring the resulting current [3]. Three electrodes are required for this purpose: 
The working electrode (WE): it is in contact with the analyte and induces the potential. The reference electrode (RE): has a known potential so that the potential difference between it and the WE can be measured.Auxiliary electrode (CE): supplies the necessary current to maintain a constant WE. 
This measurement is made by means of what is called a triangular sweep because the potential between the WE and the RE varies until it reaches an established one and then changes direction, and so on for a certain number of cycles. Below is an example of a graph obtained from cyclic voltammetry of the comparison of Sn redissolution in the modified Pt/Sn electrodes (see Figure 4). 





### Chronoamperometry 

*Chronoamperometry* (CA) consists of the study of the variation of the current response with time as a function of the control of a potentiostat. The working electrode, which is in contact with the electrolyte, is subjected to a constant change of potential from an initial to a final value for a given time [4]. The following image shows an example of a graph of a chronoamperometry in a H 2 SO 4 /ACN/CuSO 4 solution (see Figure 5).





Thus, these two electrochemical measurements are the ones we will perform on two samples of *potassium ferricyanide* at concentrations of 1 mM and 5 mM *potassium chloride*. 



### Git and GitHub 

This project is based on team code development. For this there are some tools that can facilitate version control among team members: Git and GitHub. Git is a very powerful open source code version control tool. A complete version history can be kept and it allows us to move in a very agile way through all the code revisions. In addition, it has a branching system that allows a correct organization and the realization of projects that converge in a main project, which allow testing codes before joining them [5]. 

For our project, four branches have been developed: 

- `Develop`: where all the final tested code will be put together. 
- `feature/crono`: for chronoamperometry
- `feature/volta`: for voltammetry 
- `feature/setup`: to configure all the initial pins. 

Although there are two branches for chronoamperometry and cyclic voltammetry, it should be noted that the correct and final code for both was done directly in the `develop` branch due to a technical glitch. 

As for GitHub, it is a free application that allows you to manage Git repositories and contains a web interface to easily access and control the different versions of code. 



## Objectives 

The objectives of this project are as follows:

- To program a portable potentiostat.
- Control the Power Management Unit (PMU) of the potentiostat front-end module.
- Communicate with the viSens-S application installed with the host or computer through the MASB-COMM-S protocol.
- Perform cyclic voltammetry.
- Perform chronoamperometry.



## Project Development 



### *Workflow* of the cyclic voltammetry 

First, we define and set the values of the following variables:

- `tomarPunto_CV` = `TRUE`
- We defined the `RTIA` resistance of 10,000 Ω.
- We set the cell voltage `Vcell` to `eBegin` that we get from the `cvConfiguration`.
- We set `vObjectivo` to `eVertex1`
- We obtain the `eStep` and `scanRate` from the `cvConfiguration` and define the period between samples, dividing both parameters.
- We define and initialize the `cycles` variable to 0.

We close the relay and, in case the number of cycles has not exceeded the one set in the `cvConfiguration` and the sampling period has elapsed, we start the `Vcell` and `Icell` measurements and send the obtained values to the *host*.

To start these measurements, we have used the boolean `tomarPunto_CV` in a conditional *if*, so that if this variable is `TRUE`, we will start these measurements. Right after the conditional we will change the value of the variable from `TRUE` to `FALSE`, which to make the next measurement will return to `TRUE` using one of the functions defined at the end of the *chronoamperometry* code. 

What we do from now on is a triangular sweep. We will start by applying an `eBegin` voltage to the cell and apply `eStep` increments or decrements to this voltage until we reach the voltage of `eVertex1` and, once we reach this, we will do the same until we reach the value of `eVertex2`. Once we reach this, we will do another sweep until we reach `eBegin`. All this process corresponds to a cycle, which we will have to repeat as many times as indicated by the cycles on the `cvConfiguration` .

The following image shows the aforementioned process (see Figure 6).





### *Workflow* of the chronoamperometry 

For chronoamperometry we must also define some variables:

- `tomarPunto` = `FALSE`.
- `RTIA` = `50,000 Ω
- Set `Vcell` to `eDC` which we get from the `caConfiguration`. 
- Set `sampling_period` and `measurement_time` to the same parameters of `caConfiguration`.
- We define the `counter` and `point` variables and initialize them to 0 and 1 respectively. 

In this case we close the relay and take a first measurement of `Vcell` and `Icell`, which will later be sent to the *host*. 

Next, the `counter` and `point` variables will sum the `sampling_period` and one unit, respectively, since we will have taken a sample. That is, the `counter` indicates the number of samples and the `sampling_period` the time each sample takes. 

Once the first sample has been taken, in case the `counter` has not reached the `measurement_time`, we will continue taking samples, which will be sent to the *host*.  

Finally, when the *timer* reaches the `measurement_time`, we will have taken all the measurements and the relay will open. 

Below is a schematic of the previously explained work flow for a better understanding (see *Figure* *7*).



***Figure 7***. Flowchart followed to program the microcontroller when performing chronoamperometry.



## Results 

This section shows the results obtained when testing the project in the potentiostat with two samples of *potassium ferricyanide* at concentrations of 1mM and 5mM of *potassium chloride*. 

For this purpose, we went to the Biomedical Engineering laboratory of the physics faculty of the University of Barcelona, where we were given the potentiostat, a sensor and the solutions, and where we were able to assemble everything (see *Figure 8*).





***Figure 8.*** Potentiostat connected with the sample of the solution in the sensor.



We first proceeded to perform cyclic voltammetry using the viSens-S program, setting the following parameters:

- E begin (V):
- E vertex 1 (V):
- E vertex 2 (V):
- Cycles:
- Scan rate (V/s):
- E step (V):

Unfortunately, cyclic voltammetry did not work. Only one measurement was taken, which is insufficient. 



Next, chronoamperometry was performed with the same program and the following values:

- E DC (V): 0'15
- Sampling period (ms): 20 
- Measurement time (ms): 10

On this occasion it was possible to visualize the results, although in the case of the 1 mM potassium chloride concentration sample, the current values were inverted, as can be seen in the following image (see *Figure 9*).





***Figure 9***. Chronoamperometry with 1 mM solution.



In the case of the following sample, the results are satisfactory, as can be seen in the following graph (see *Figure 10*).



***Figure 10***. Chronoamperometry with 5 mM dilution



## Conclusions

The potentiostat is an instrument widely used in electrochemical experiments and is key in the world of biomedical engineering to measure different concentrations of relevant substances that allow us to determine the level of health of an individual. This project allows us to have a first contact with the programming of a microcontroller for this instrument and, also, allows us to use the knowledge obtained throughout all the practicals performed in this subject.

Considering that the rest of the practices were about different aspects of programming a microcontroller individually and this practice integrates most of them, this project has presented several difficulties and complications. Especially when creating different code files for each of the features and running them together and also when using GitHub to access and modify the files shared with the partner.



## References