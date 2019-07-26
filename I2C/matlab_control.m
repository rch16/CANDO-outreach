% Create a serial port value on x and takes in properties of the object as arguments
% USB port = /dev/cu.usmodem14201
% BAUD of the object = 9600
x = serial('/dev/cu.usbmodem14201', 'BAUD', 9600);

fopen(x);

go = true;

while go
    a = input('Input 1 to start motor, Input 0 to finish: ');
    
    if (a == 0)
        % If user inputs "stop", motor stops
        fprintf('stopping motor\n');
        go = false;
    else if (a == 1)
        % If user inputs "go", motor starts
        fprintf('starting motor\n');
    else
        % Invalid input, do nothing
    end
    end
end

fclose(x);
