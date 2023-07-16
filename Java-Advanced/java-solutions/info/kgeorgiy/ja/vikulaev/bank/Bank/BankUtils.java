package info.kgeorgiy.ja.vikulaev.bank.Bank;

public class BankUtils {
    public static final String CREDENTIALS_ERROR = "Wrong person credentials." +
            "\nFirstname and last name has to contain only letters, passport has to contain only numbers.";
    public static boolean checkPersonCredentials(final String firstname, final String lastname, final String passport) {
        if (passport == null || firstname == null || lastname == null) {
            return false;
        }
        return passport.matches("[0-9]+") && firstname.matches("[A-Z][a-z]+")
                && lastname.matches("[A-Z]+([ '-][a-zA-Z]+)*[a-zA-Z]+");
    }

    public static String fullName(final String id, final String passportID) {
        return passportID+ ":" + id;
    }
}
