% Create a serial port value on x and takes in properties of the object as arguments
% USB port = /dev/cu.usmodem14101
% BAUD of the object = 9600
x = serial('/dev/cu.usbmodem14201', 'BAUD', 9600);

fopen(x);

go = true;

while go
    a = input('Input "go" to start motor, Input "stop" to finish: ');
    
    if (a == "stop")
        % If user inputs "stop", motor stops
        go = false;
    else if (a == "go")
        % If user inputs "go", motor starts
        fprintf("Starting motor", '%s');
    else
        % Invalid input, do nothing
    end
end

fclose(x);
