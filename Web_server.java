import java.io.*;
import java.net.*;
import java.lang.String;
import java.util.*;
import java.text.SimpleDateFormat;

public class Web_server {
    private Socket socket;

    private Web_server(){                                                                // Constructor
        //in = null;
        socket = null;
        //serverSocket = null;
        //reader = null;
    }

    private void setup(int portNum){                                                               // setup the server socket and port number
        try {
            ServerSocket serverSocket = new ServerSocket(portNum);
            System.out.println("Waiting for client to connect\n");
            socket = serverSocket.accept();
            receive_request();                                                          // receive a request from client
        } catch (IOException e) {
            System.out.println("File Transfer Failed");
        }
        finally{                                                                        // terminate the connection after file transfer
            try {
                socket.close();
                socket = null;
            } catch (IOException e) {
                System.out.println("Connection Termination Failed");
            }
            System.out.println("TCP Connection Terminated");
        }
    }

    private void receive_request(){                                                     // receive request, determine file name and http version
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            String current_line = in.readLine();
            String header_line = current_line;
            String client_req = "";

            while(in.ready()){
                System.out.println(current_line);
                client_req += current_line;
                current_line = in.readLine();
            }
            System.out.println("\n");

            String[] reqLine = header_line.split(" ");
            String command = reqLine[0];
            String file = reqLine[1];
            String version = reqLine[2];

            if(command.equals("GET")) {
                if(version.equals("HTTP/1.1")) {
                    try {
                        file = file.substring(1);
                        send_file("HTTP/1.1 200 OK\r\n", file);                 // when http version is supported
                        in.close();
                    } catch (Exception e) {
                        System.out.println("Error Occurred");
                    }
                }
                else {
                    try {
                        file = file.substring(1);
                        System.out.println("HTTP Version Not Supported");
                        send_file("HTTP/1.1 505 HTTP Version Not Supported\r\n", file);  // when http version is not supported
                        in.close();
                    }
                    catch (Exception e) {
                        System.out.println("Error Occurred");
                    }
                }
            }
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }

    private void send_file(String status,String file) throws IOException {                                // send the file and header, status to the client
        FileReader filereader;
        PrintWriter output = null;
        try {
            File f = new File(file);
            if(f.exists()) {                                                          // if the file exists
                filereader = new FileReader(f);
                BufferedReader reader = new BufferedReader(filereader);
                output = new PrintWriter(new BufferedWriter(
                        new OutputStreamWriter(socket.getOutputStream())));

                String form = f.getName();
                int posi = f.getName().indexOf('.');
                form = form.substring(posi + 1);
                String response = "";
                response += status;                                               // send the status line
                if (status.equals("HTTP/1.1 200 OK\r\n")) {                           // 200 will enter this section, 505 by default will not enter
                    String current_time = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss").format(Calendar.getInstance().getTime());
                    response += "Date: " + current_time + "\r\n";

                    response += "Server: Maoshun's Web Server\r\n";
                    response += "Content-Length: " + f.length() + "\r\n";
                    response += "Content-Type: " + form + "\r\n";
                    response += "Connection: Closed\r\n\r\n";
                    String content;
                    while ((content = reader.readLine()) != null) {
                        response += (content + "\r\n");
                    }
                }
                output.print(response);
                output.flush();
                System.out.println(response);
            }
            else{                                                                     // if file not exist (404)
                output = new PrintWriter(new BufferedWriter(
                        new OutputStreamWriter(socket.getOutputStream())));
                output.print("HTTP/1.1 404 Not Found \r\n");
                System.out.println("HTTP/1.1 404 Not Found \r\n");
            }
        }
        catch (IOException e) {
            System.out.println(e.getMessage());
        }
        finally{
            assert output != null;
            output.close();
        }
    }
    public static void main(String args[]){                                             // main method
        int[] queue = {11111,11112,11113,11114,11115,11116};
        int points_to = 0;
        Web_server server;
        while(queue.length <= 6 && queue.length >= 0) {
            server = new Web_server();
            server.setup(queue[points_to]);
            points_to = (points_to + 1) % 6 ;
        }
    }
}
