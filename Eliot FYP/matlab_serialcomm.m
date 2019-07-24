% Create a serial port value on x and takes in properties of the object as arguments
% USB port = /dev/cu.usmodem14101
% BAUD of the object = 9600
x = serial('/dev/cu.usbmodem14101', 'BAUD', 9600);

fopen(x);

go = true;

while go
    a = input('Input the number of connected terminals, Press 0 to break: ');
    
    if (a == 0)
        % If user inputs 0, choreography finishes
        go = false;
    else
        % Formats input as a single character
        fprintf(x, '%c', a);
    end
end

fclose(x);
