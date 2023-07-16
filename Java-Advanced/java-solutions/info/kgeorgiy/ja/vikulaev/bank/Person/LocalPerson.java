package info.kgeorgiy.ja.vikulaev.bank.Person;

import info.kgeorgiy.ja.vikulaev.bank.Account.Account;
import info.kgeorgiy.ja.vikulaev.bank.Account.LocalAccount;

import java.rmi.RemoteException;
import java.util.Map;

public class LocalPerson extends AbstractPerson {

    private final Map<String, LocalAccount> accounts;

    public static LocalPerson newLocalPerson(final String firstName, final String lastName, final String passportID,
                                        Map<String, LocalAccount> accounts) {
        return new LocalPerson(firstName, lastName, passportID, accounts);
    }

    private LocalPerson(final String firstName, final String lastName, final String passportID, Map<String, LocalAccount> accounts) {
        super(firstName, lastName, passportID);
        this.accounts = accounts;
    }

    @Override
    protected void create(final String fullName) throws RemoteException {
        accounts.put(fullName, new LocalAccount(fullName, 0, getPassportID()));
    }

    @Override
    public Account getAccount(final String id) {
        return accounts.get(fullName(id));
    }

    @Override
    public boolean accountExists(final String id) {
        return accounts.containsKey(fullName(id));
    }
}
