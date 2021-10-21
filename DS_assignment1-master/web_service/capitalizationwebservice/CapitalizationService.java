package capitalizationwebservice;
import javax.jws.WebService;
import javax.jws.WebMethod;


@WebService()
public class CapitalizationService {

    @WebMethod()
    public String capitalize(String message){
        String [] words = message.split(" "); //split the massage into words
        String capitalized = "";
        for(String word: words){ //for loop to transform all the words into Uppercase
            word = word.trim(); // Remove extra spaces
            if(word.length()<1)  // If word length is 0, go to the next word.
                continue;
            capitalized+=Character.toUpperCase(word.charAt(0))+word.substring(1)+" ";
            //append into the string the words with upperCase
        }
        return capitalized; //return string
    }

}
