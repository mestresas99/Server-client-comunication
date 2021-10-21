import java.io.*;
import gnu.getopt.Getopt;

import wsclient.CapitalizationService;
import wsclient.CapitalizationServiceService;

import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.ServerSocket;

class IOUtils {
//readline function to read a string received from a sender user
    public static String readLine(DataInputStream inputSocket){
        byte[]  ch = new byte[1];
        String message="";
        do{
            try{
                ch[0] = inputSocket.readByte();
            }catch(IOException e){
                System.err.println("Error reading string: "+e.getMessage());
            }
            if(ch[0]!= '\0'){
                String answer = new String(ch);
                message+=answer;

            }
        }while(ch[0] != '\0');

        return message;
    }
}
//Object that implements threading in the client side
class ClientServer implements Runnable {
    private final ServerSocket serverSocket;
    private boolean exit = false;
    String name;
    //We create a struct with serverSocket and a name
    public ClientServer(ServerSocket serverSocket, String name){
        this.serverSocket = serverSocket;
        this.name = name;
    }

    public void run(){
        //Loop that doesnt end until the client has disconnected
        while(!exit){
            Socket clientSocket;
            try{
            clientSocket = serverSocket.accept();
            }catch(IOException e){
                if (!exit){
                    System.err.println("Error accepting a client connection: "+e.getMessage());
                }
                return;
            }
            //We create the input/output sockets
            DataInputStream inputSocket;
            DataOutputStream outputSocket;
            try{
                outputSocket =new DataOutputStream(clientSocket.getOutputStream());
                inputSocket =new DataInputStream(clientSocket.getInputStream());

            }catch(IOException e){
                System.err.println("Error creating buffer: "+e.getMessage());
                return;
            }
            //2 possible operations
            String operation =  IOUtils.readLine(inputSocket);
            if (operation.equals("SEND_MESSAGE")){
                //Client has been sent a message (either itś recived instantly if he was connected or if he was disconnected right when he connected )
                String userName = IOUtils.readLine(inputSocket);
                String message_identifier = IOUtils.readLine(inputSocket);
                String message = IOUtils.readLine(inputSocket);
                System.out.print(" MESSAGE "+message_identifier+" FROM "+userName+":\n\t "+message+"\nEND\nc>");
            }else if(operation.equals("SEND_MESS_ACK")){
                //Confirmation that the client has recieved the messaged (kind of double tick in messanger services)
                String message_identifier = IOUtils.readLine(inputSocket);
                System.out.print(" SEND MESSAGE "+message_identifier+" OK\nc>");

            }
        }
    }
    public void stop(){
    exit = true;
    }
}

class client {
	
	/********************* TYPES **********************/
	
	/**
	 * @brief Return codes for the protocol methods
	 */
	private static enum RC {
		OK,
		ERROR,
		USER_ERROR	
	};
	
	/******************* ATTRIBUTES *******************/
	//If conectionStatus = 0 -> is not conected, else its connected
    private static String connectionName = null;
   	private static int connectionStatus = 0;
    private static String _server   = null;
    private static int _port = -1;
    private static ServerSocket clientSocket;
    private static Thread clientServer;
    private static ClientServer server = null;
    private static CapitalizationServiceService service = new CapitalizationServiceService();
    private static CapitalizationService port;

	
	/********************* METHODS ********************/
	
	/**
	 * @param user - User name to register in the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user is already registered
	 * @return ERROR if another error occurred
	 */
	static RC register(String user) throws IOException{
        //We asume that the client length cannot be longer that 256, so we check the lenght
        RC rc= RC.USER_ERROR;
        if( user.length()>256 ){
            System.out.println("Username is longer than 256 characters");
            return rc;
        }

        //We create the sockets

        Socket serverSocket = null;
        DataOutputStream outputSocket;
        DataInputStream inputSocket;

        try{
            InetAddress addr = InetAddress.getByName(_server);
            serverSocket = new Socket(addr, _port);

        }catch(SocketException e){
            System.err.println("Error creating socket: "+e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
        outputSocket = new DataOutputStream(serverSocket.getOutputStream());
        inputSocket = new DataInputStream(serverSocket.getInputStream());
        String message;
        try{
            //wE SEND THE operation
            String operation = "REGISTER\0";
            outputSocket.writeBytes(operation);
            //outputSocket.write('\0');
            //We add '/0' at the end

            //Username
            String username = user + "\0";
            outputSocket.writeBytes(username);

            //Server answer
            //We read byte by byte until we find '/0' which means its over
            message=IOUtils.readLine(inputSocket);


        }catch(IOException e){
            message = "2";
            rc=RC.ERROR;
            System.err.println("ERROR WRITING/READING BYTES: "+ e.getMessage());
            e.printStackTrace();
        }finally{
            serverSocket.close();
            outputSocket.close();
            inputSocket.close();
        }
        //We print in the terminal according to the result
        switch (Integer.parseInt(message)){
            case 0: System.out.println("c> REGISTER OK");
            rc=RC.OK;
            break;
            case 1: System.out.println("c> USERNAME IN USE");
            break;
            case 2: System.out.println("c> REGISTER FAIL");
            break;
        }
        // We close the sockets and the communication


        return rc;
    }
	
	/**
	 * @param user - User name to unregister from the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist
	 * @return ERROR if another error occurred
	 */
	static RC unregister(String user) 
	{

        RC rc=RC.USER_ERROR;;
		//We asume that the client length cannot be longer that 256, so we check the lenght

		if(user.length()>256){
			System.out.println("Username is longer than 256 characters");
            return rc;
		}

		//We create the sockets

		Socket serverSocket = null;
		DataOutputStream outputSocket;
		DataInputStream inputSocket;

		try{
			serverSocket = new Socket(_server, _port);
		}catch(Exception e){
			System.out.println("Error connecting with server.");
            rc = RC.ERROR;
            return rc;
		}
		String message;
		try{
            outputSocket = new DataOutputStream(serverSocket.getOutputStream());
            inputSocket = new DataInputStream(serverSocket.getInputStream());
			//wE SEND THE operation
			String operation = "UNREGISTER\0";
			outputSocket.writeBytes(operation);
			//We add '/0' at the end
			String username = user + '\0';
			//Username

			outputSocket.writeBytes(user);
			outputSocket.write('\0');

			//Server answer
			//We read byte by byte until we find '/0' which means its over

			message=IOUtils.readLine(inputSocket);
            serverSocket.close();
            outputSocket.close();
            inputSocket.close();
		}catch(java.io.IOException e){
			message = "2";
			rc = RC.ERROR;
		}
		//We print in the terminal according to the result
		switch (Integer.parseInt(message)){
			case 0: System.out.println("c> UNREGISTER OK");
				rc=RC.OK;
				break;
			case 1: System.out.println("c> USERNAME DOES NOT EXIST");
				break;
			case 2: System.out.println("c> UNREGISTER FAIL");

				break;
		}
		// We close the sockets and the communication



		return rc;
	}
	
    /**
	 * @param user - User name to connect to the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist or if it is already connected
	 * @return ERROR if another error occurred
	 */
	static RC connect(String user) 
    	{
            RC rc = RC.USER_ERROR;
    		//We first check if the client is already connected
    		if(connectionStatus != 0){
    			System.out.println(" Client already connected");
                rc = RC.USER_ERROR;
                return rc;
    		}


        	if(user.length()>256){
        	    System.out.println("Username is longer than 256 characters");
                rc = RC.USER_ERROR;
                return rc;
            }

                //We create the sockets

            Socket serverSocket = null;
            DataOutputStream outputSocket;
            DataInputStream inputSocket;

            try{
                serverSocket = new Socket(_server, _port);
            }catch(Exception e){
                System.err.println("Error connecting with server.");
                rc= RC.ERROR;
                return rc;
            }

            String message;


    		//We  create the server socket for the client's server, must be in a thread
    		try{
    		    clientSocket = new ServerSocket(0);
    		}catch(IOException e){
    		    System.err.println("Error creating clientSocket: "+e.getMessage());
                rc = RC.ERROR;
                return rc;
                }
    		//server's client is created
    		server = new ClientServer(clientSocket,user);
    		clientServer = new Thread(server);
    		//Thread is initiated
    		clientServer.start();

    		try{
                outputSocket = new DataOutputStream(serverSocket.getOutputStream());
                inputSocket = new DataInputStream(serverSocket.getInputStream());
    			String operation = "CONNECT\0";
    			outputSocket.writeBytes(operation);

    			String username = user + "\0";
    			outputSocket.writeBytes(username);

    			//Now we send the port
    			String port = Integer.toString(clientSocket.getLocalPort()) + "\0";
    			outputSocket.writeBytes(port);

    			//Server answer
                //We read byte by byte until we find '/0' which means its over

                message=IOUtils.readLine(inputSocket);
                serverSocket.close();
                outputSocket.close();
                inputSocket.close();
            }catch(java.io.IOException e){
                message = "3";
                rc = RC.ERROR;
            }
            boolean closeClientServer=false;
    		switch (Integer.parseInt(message)){
    			case 0: System.out.println("c> CONNECT OK");
    				//We switch connection status to 1
    				connectionStatus = 1;
    				connectionName = user;
    				rc=RC.OK;
    				break;
    			case 1: System.out.println("c> CONNECT FAIL, USER DOES NOT EXIST");
                    closeClientServer=true;
    				break;
    			case 2: System.out.println("c> USER ALREADY CONNECTED");
                    closeClientServer=true;
    				break;
    			case 3: System.out.println("c> CONNECT FAIL");
    				break;
    		}
            if (closeClientServer){
                try{
                    server.stop();
                    clientSocket.close();

                }catch(IOException e){
                    System.out.println("Error closing the sockets");
                    rc= RC.ERROR;
                    return rc;
                }
            }



    		return rc;
    	}

    	 /**
    	 * @param user - User name to disconnect from the system
    	 *
    	 * @return OK if successful
    	 * @return USER_ERROR if the user does not exist
    	 * @return ERROR if another error occurred
    	 */
    	static RC disconnect(String user)
    	{
    			RC rc = RC.USER_ERROR;
    		//We first check if the client is already connected
    		if(connectionStatus == 0){
    			System.out.println(" Client already disconnected");
                rc = RC.USER_ERROR;
                return rc;
    		}

            //Check username length
        	if(user.length()>256){
        	    System.out.println("Username is longer than 256 characters");
                rc = RC.USER_ERROR;
                return rc;
            }

                //We create the sockets

            Socket serverSocket = null;
            DataOutputStream outputSocket;
            DataInputStream inputSocket;

            try{
                serverSocket = new Socket(_server, _port);
            }catch(Exception e){
                System.err.println("Error connecting with server.");
                rc = RC.ERROR;
                return rc;
            }


            String message;


    		try{
    		    //Set the socket values
                outputSocket = new DataOutputStream(serverSocket.getOutputStream());
                inputSocket = new DataInputStream(serverSocket.getInputStream());
    			String operation = "DISCONNECT\0";
    			outputSocket.writeBytes(operation);

    			String username = user + "\0";
    			outputSocket.writeBytes(username);

    			//Now we send the port


    			//Server answer
                //We read byte by byte until we find '/0' which means its over

                message=IOUtils.readLine(inputSocket);
                serverSocket.close();
                outputSocket.close();
                inputSocket.close();

            }catch(java.io.IOException e){
                message = "3";
                rc= RC.ERROR;
            }
    		switch (Integer.parseInt(message)){
    			case 0: System.out.println("c> DISCONNECT OK");
    				//We switch conection status to 0

                    connectionStatus = 0;
                    connectionName=null;
                    rc=RC.OK;

                    try{
                        server.stop();
                        clientSocket.close();
                    }catch(IOException e){
                        System.out.println("Error closing socket");
                        rc=RC.ERROR;
                    }
                    break;
    			case 1:
    				System.out.println("c> DISCONNECT FAIL, USER DOES NOT EXIST");
    				break;
    			case 2:
    				System.out.println("c> USER ALREADY DISCONNECTED");
    				break;
    			case 3: System.out.println("c> DISCONNECT FAIL");
    				break;
    		}



    		return rc;
    	}

	 /**
	 * @param user    - Receiver user name
	 * @param message - Message to be sent
	 * 
	 * @return OK if the server had successfully delivered the message
	 * @return USER_ERROR if the user is not connected (the message is queued for delivery)
	 * @return ERROR the user does not exist or another error occurred
	 */
	static RC send(String user, String message) 
	{
	    RC rc = RC.USER_ERROR;
        //We first check if the client is already connected
        if(connectionStatus == 0){
            System.out.println(" Client is not connected");

            return rc;
        }
        if(user.length()>256){
            System.out.println("Username is longer than 256 characters");
            return rc;
        }
        if(!(message.length()>0 && message.length()<256)){
            System.out.println("Message length is not correct");
            return rc;
        }
        //We create the sockets
        Socket serverSocket = null;
        DataOutputStream outputSocket;
        DataInputStream inputSocket;
        try{
            serverSocket = new Socket(_server, _port);
        }catch(Exception e){
            System.out.println("Error");
            rc= RC.ERROR;
            return rc;
        }
        String answer;
        String answer_id = "";
        try{
            outputSocket = new DataOutputStream(serverSocket.getOutputStream());
            inputSocket = new DataInputStream(serverSocket.getInputStream());
            String operation = "SEND\0";
            outputSocket.writeBytes(operation);

            String sender = connectionName + "\0";
            outputSocket.writeBytes(sender);

            String destination = user + "\0";
            outputSocket.writeBytes(destination);

            //Capitalization function from the web service
            message=port.capitalize(message);
            String message_pk = message + "\0";
            outputSocket.writeBytes(message_pk);

            answer=IOUtils.readLine(inputSocket);
            if(Integer.parseInt(answer)==0)
                answer_id=IOUtils.readLine(inputSocket);

            serverSocket.close();
            outputSocket.close();
            inputSocket.close();
        }catch(java.io.IOException e){
            answer = "2";
            rc = RC.ERROR;

        }
        boolean closeClientServer=false;
        switch (Integer.parseInt(answer)){
            case 0:
                System.out.println(" SEND OK - MESSAGE " + answer_id);
                rc=RC.OK;
                break;
            case 1:
                System.out.println(" SEND FAIL / USER DOES NOT EXIST");
                break;
            case 2:
                System.out.println(" SEND FAIL ");
                break;
        }

		// Write your code here
		return rc;
	}
	
	/**
	 * @brief Command interpreter for the client. It calls the protocol functions.
	 */
	static void shell() 
	{
		boolean exit = false;
		String input;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

		while (!exit) {
			try {
				System.out.print("c> ");
				input = in.readLine();
				line = input.split("\\s");

				if (line.length > 0) {
					/*********** REGISTER *************/
					if (line[0].equals("REGISTER")) {
						if  (line.length == 2) {
							register(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: REGISTER <userName>");
						}
					} 
					
					/********** UNREGISTER ************/
					else if (line[0].equals("UNREGISTER")) {
						if  (line.length == 2) {
							unregister(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: UNREGISTER <userName>");
						}
                    } 
                    
                    /************ CONNECT *************/
                    else if (line[0].equals("CONNECT")) {
						if  (line.length == 2) {
							connect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: CONNECT <userName>");
                    	}
                    } 
                    
                    /********** DISCONNECT ************/
                    else if (line[0].equals("DISCONNECT")) {
						if  (line.length == 2) {
							disconnect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: DISCONNECT <userName>");
                    	}
                    } 
                    
                    /************** SEND **************/
                    else if (line[0].equals("SEND")) {
						if  (line.length >= 3) {
							// Remove first two words
							String message = input.replaceFirst("SEND "+line[1]+" ", "");
							send(line[1], message); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: SEND <userName> <message>");
                    	}
                    } 
                    
                    /************** QUIT **************/
                    else if (line[0].equals("QUIT")){
						if (line.length == 1) {
							exit = true;
						} else {
							System.out.println("Syntax error. Use: QUIT");
						}
					} 
					
					/************* UNKNOWN ************/
					else {						
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}				
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * @brief Prints program usage
	 */
	static void usage() 
	{
		System.out.println("Usage: java -cp . client -s <server> -p <port>");
	}
	
	/**
	 * @brief Parses program execution arguments 
	 */ 
	static boolean parseArguments(String [] argv) 
	{
		Getopt g = new Getopt("client", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				//case 'd':
				//	_debug = true;
				//	break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Integer.parseInt(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}
		
		if (_server == null)
			return false;
		
		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}
	
	
	
	/********************* MAIN **********************/
	
	public static void main(String[] argv) 
	{
 		if(!parseArguments(argv)) {
 			usage();
 			return;
 		}

 		// Write code here
        port = service.getCapitalizationServicePort();
 		shell();



		System.out.println("+++ FINISHED +++");
		System.exit(0);
	}
}
