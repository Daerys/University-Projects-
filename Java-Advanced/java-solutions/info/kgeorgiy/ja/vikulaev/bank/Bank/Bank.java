package info.kgeorgiy.ja.vikulaev.bank.Bank;

import info.kgeorgiy.ja.vikulaev.bank.Account.Account;
import info.kgeorgiy.ja.vikulaev.bank.Person.LocalPerson;
import info.kgeorgiy.ja.vikulaev.bank.Person.Person;
import info.kgeorgiy.ja.vikulaev.bank.Person.RemotePerson;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface Bank extends Remote {

    /**
     * Creates a new account with specified identifier if it does not already exist and memorize to which
     * user this account belongs to.
     *
     * @param id account id
     * @return created or existing account.
     */
    Account createAccount(final String id, final String passportID) throws RemoteException;

    /**
     * Returns account by identifier.
     *
     * @param id account id
     * @return account with specified identifier or {@code null} if such account does not exist.
     */
    Account getAccount(String id) throws RemoteException;

    /**
     * Creates a new person with the specified passport, first name, and last name.
     *
     * @param passport  the passport number of the person
     * @param firstname the first name of the person
     * @param lastname  the last name of the person
     * @return the created person
     */
    Person createPerson(final String firstname, final String lastname, final String passport);

    /**
     * Retrieves the remote person with the specified passport.
     *
     * @param passport the passport number of the person
     * @return the remote person with the specified passport
     */
    RemotePerson getRemotePerson(String passport);

    /**
     * Retrieves the local person with the specified passport.
     *
     * @param passport the passport number of the person
     * @return the local person with the specified passport
     */
    LocalPerson getLocalPerson(String passport);

    /**
     * Checks if an account with the specified identifier exists.
     *
     * @param id the identifier of the account
     * @return {@code true} if the account exists, {@code false} otherwise
     */
    boolean accountExists(String id);
}
