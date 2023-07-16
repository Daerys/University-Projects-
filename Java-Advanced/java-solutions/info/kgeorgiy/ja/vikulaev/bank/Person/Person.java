package info.kgeorgiy.ja.vikulaev.bank.Person;

import info.kgeorgiy.ja.vikulaev.bank.Account.Account;

import java.rmi.RemoteException;

/**
 * The {@code Person} interface represents a remote person with basic information and account management.
 */
public interface Person {

    /**
     * Returns the first name of the person.
     */
    String getFirstname() throws RemoteException;

    /**
     * Returns the last name of the person.
     */
    String getLastname() throws RemoteException;

    /**
     * Returns the passport ID of the person.
     */
    String getPassportID() throws RemoteException;

    /**
     * Returns the full name of the person identified by the specified ID.
     */
    String fullName(String id) throws RemoteException; // :NOTE: it is not fullName

    /**
     * Returns the account associated with the specified ID.
     */
    Account getAccount(String id) throws RemoteException;

    /**
     * Changes the amount of the account associated with the specified ID.
     */
    void changeAccountAmount(String id, int amount) throws RemoteException;
}
