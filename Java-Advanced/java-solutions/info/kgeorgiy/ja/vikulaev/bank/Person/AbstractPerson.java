package info.kgeorgiy.ja.vikulaev.bank.Person;

import info.kgeorgiy.ja.vikulaev.bank.Account.Account;

import java.rmi.RemoteException;

public abstract class AbstractPerson implements Person {
    private final String firstName;
    private final String lastName;
    private final String passportID;

    /**
     * Constructs an {@code AbstractPerson} object with the specified first name, last name, and passport ID.
     *
     * @param firstName  the first name of the person
     * @param lastName   the last name of the person
     * @param passportID the passport ID of the person
     */
    public AbstractPerson(String firstName, String lastName, String passportID) {
        this.firstName = firstName;
        this.lastName = lastName;
        this.passportID = passportID;
    }

    /**
     * Returns the full name of the person combined with the given ID.
     *
     * @param id the ID to be combined with the passport ID
     * @return the full name of the person combined with the ID
     */
    public String fullName(String id) {
        return String.format("%s:%s", passportID, id);
    }

    @Override
    public String getFirstname() throws RemoteException {
        return firstName;
    }

    @Override
    public String getLastname() throws RemoteException {
        return lastName;
    }

    @Override
    public String getPassportID() throws RemoteException {
        return passportID;
    }

    /**
     * Creates an account with the specified ID.
     *
     * @param id The ID of the account.
     * @return The created account.
     * @throws RemoteException If a remote exception occurs during the operation.
     */
    public Account createAccount(final String id) throws RemoteException {
        String fullName = fullName(id);
        if (!accountExists(fullName)) {
            create(fullName);
        }
        return getAccount(fullName);
    }

    /**
     * Creates an account with the specified full name.
     * This method should be implemented by subclasses.
     *
     * @param fullName The full name of the account.
     * @throws RemoteException If a remote exception occurs during the operation.
     */
    protected abstract void create(final String fullName) throws RemoteException;

    /**
     * Checks if an account with the specified ID exists.
     * This method should be implemented by subclasses.
     *
     * @param id The ID of the account.
     * @return {@code true} if the account exists, {@code false} otherwise.
     */
    public abstract boolean accountExists(String id) throws RemoteException;

    @Override
    public void changeAccountAmount(final String id, int amount) throws RemoteException {
        Account account = this.getAccount(id);
        account.setAmount(account.getAmount() + amount);
    }
}
