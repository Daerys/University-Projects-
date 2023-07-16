package info.kgeorgiy.ja.vikulaev.bank.Person;

import info.kgeorgiy.ja.vikulaev.bank.Account.Account;
import info.kgeorgiy.ja.vikulaev.bank.Bank.RemoteBank;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RemotePerson extends AbstractPerson implements Remote {

    private final RemoteBank bank;

    public static RemotePerson newRemotePerson(String firstName, String lastName, String passportID, int port, final RemoteBank bank) throws RemoteException {
        RemotePerson person = new RemotePerson(firstName, lastName, passportID, bank);
        UnicastRemoteObject.exportObject(person, port);
        return person;
    }
    private RemotePerson(String firstName, String lastName, String passportID, final RemoteBank bank) throws RemoteException {
        super(firstName, lastName, passportID);
        this.bank = bank;
    }

    @Override
    protected void create(final String fullName) throws RemoteException {
        bank.createAccount(fullName, this.getPassportID());
    }

    @Override
    public Account getAccount(final String id) throws RemoteException {
        return bank.getAccount(fullName(id));
    }

    @Override
    public boolean accountExists(final String id) throws RemoteException {
        return bank.accountExists(fullName(id));
    }
}
