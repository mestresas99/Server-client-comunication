package capitalizationwebservice;
import javax.jws.WebService;
import javax.jws.WebMethod;
import javax.xml.ws.Endpoint;

public class WebPublisher {

	public static void main(String[] argv){
	    final String url = "http://localhost:8888/rs"; //get the local IP
        System.out.println("Publishing Webservice at endpoint: " + url);
        Endpoint.publish(url, new CapitalizationService()); //calling the capitalization service method to change from lowe case to upper case with the specific IP
	}
}
