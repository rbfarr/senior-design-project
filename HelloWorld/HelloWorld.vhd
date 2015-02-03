LIBRARY ieee;

USE ieee.std_logic_1164.all;

ENTITY HelloWorld IS
	PORT( x1, x2: IN	STD_LOGIC ;
			f		: OUT	STD_LOGIC );
END HelloWorld ;

ARCHITECTURE LogicFunction of HelloWorld IS
BEGIN
	f <= (x1 AND NOT x2) or (NOT x1 AND x2);
END LogicFunction ;
