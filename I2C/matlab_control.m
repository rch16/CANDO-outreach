% Create a serial port value on x and takes in properties of the object as arguments
x = serial('/dev/cu.usbmodem14201', 'BAUD', 9600);
fopen(x);

go = true;

while go
    
    % Launch GUI
    %GUI;
    
    a = input('Input 1 to start motor, Input 0 to finish: ');
   
    if (a == 0)
        % If user inputs "stop", motor stops
        fprintf(x, '%c', a);
        go = false;
        
    else if (a == 1)
        % If user inputs "go", motor starts
        fprintf(x, '%c', a);
        
    else
        % Invalid input, do nothing
    end
    end
end

fclose(x);

