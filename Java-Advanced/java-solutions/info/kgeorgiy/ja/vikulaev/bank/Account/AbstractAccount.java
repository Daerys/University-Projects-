package info.kgeorgiy.ja.vikulaev.bank.Account;

import java.rmi.RemoteException;

public class AbstractAccount implements Account {
    private final String id;
    private final String passportId;
    private int amount;

    public AbstractAccount(final String id, final String passportId) {
        this(id, passportId, 0);
    }

    public AbstractAccount(final String id, final String passportId, int amount) {
        this.id = id;
        this.passportId = passportId;
        this.amount = amount;
    }

    @Override
    public String getPassportID() throws RemoteException {
        return passportId;
    }

    @Override
    public String getId() throws RemoteException { // :NOTE: throws RemoteException (Вавжно для RemoteAccount)
        return id;
    }

    @Override
    public synchronized int getAmount() throws RemoteException {
        System.out.println("Getting amount of money for account " + id);
        return amount;
    }

    @Override
    public synchronized void setAmount(final int amount) throws RemoteException {
        System.out.println("Setting amount of money for account " + id);
        this.amount = amount;
        System.out.printf("New balance of %s account: %d%n", id, amount);
    }
}
