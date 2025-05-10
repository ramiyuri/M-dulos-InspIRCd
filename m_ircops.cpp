/*
 *   cmd_ircops - /IRCOPS Comando que lista os operadores IRC
  *   (C) Copyright 2000-2005 openglx <openglx@brasnerd.com.br>
 *   (C) Copyright 2000-2005 Lamego <lamego@ptlink.net>
 *   Adaptado para InspIRCd por: 
 *   WebChat <webchat@brturbo.net>
 *   IceCube <icecube@brturbo.net> 
 *   Este programa é software livre; você pode redistribuir e/ou modificar
 *   sob os termos da Licença Pública Geral GNU, conforme publicada pela
 *   Free Software Foundation; ou a versão 2 da Licença, ou (a sua escolha)
 *   qualquer versão posterior.
 */

#include <inspircd.h>
#include <string>
#include <sstream>
#include <iomanip>

class CommandIrcops : public Command
{
 private:
	bool IsService(User* user)
	{
		return user->IsModeSet('S');
	}

	std::string GetOperClass(User* user)
	{
		if (user->IsModeSet('o')) // Oper
			return "Oper";
		else if (user->IsModeSet('h')) // Helper
			return "Helper";
		else if (user->IsModeSet('b')) // Bot
			return "Bot";
		else if (user->IsModeSet('s')) // Staff
			return "Staff";
		else if (user->IsModeSet('c')) // Scan
			return "Scan";
		else
			return "Desconhecido";
	}

 public:
	CommandIrcops(Module* parent) : Command(parent, "IRCOPS", 0, 0)
	{
		syntax = "";
	}

	CmdResult Handle(User* user, const Params& parameters) CXX11_OVERRIDE
	{
		// Cabeçalho com espaços adicionais
		user->WriteNotice("========================================================================================");
		user->WriteNotice("");  // Linha em branco antes do cabeçalho
		std::string header = InspIRCd::Format("%-15s %-20s %s", "Nick", "Status", "Servidor");
		user->WriteNotice(header);
		user->WriteNotice("----------------------------------------------------------------------------------------");
		user->WriteNotice("");  // Linha em branco após o cabeçalho

		// Contadores
		int total_opers = 0;
		int total_away = 0;

		// Corrigindo o loop para usar o iterador correto
		const user_hash& users = ServerInstance->Users.GetUsers();
		for (user_hash::const_iterator i = users.begin(); i != users.end(); ++i)
		{
			User* u = i->second;
			
			// Pula se não for oper ou se for serviço
			if (!u->IsOper() || u->server->IsULine() || u->IsModeSet('S'))
				continue;

			total_opers++;
			if (u->IsAway())
				total_away++;

			std::string operclass = GetOperClass(u);

			std::string line = InspIRCd::Format("%-15s %-20s %s", 
				u->nick.c_str(),
				operclass.c_str(),
				u->server->GetName().c_str());
			user->WriteNotice(line);
		}

		user->WriteNotice("");  // Linha em branco antes do total
		// Rodapé
		user->WriteNotice(InspIRCd::Format("Total: %d IRCOPs | Ausentes: %d", total_opers, total_away));
		user->WriteNotice("========================================================================================");

		return CMD_SUCCESS;
	}
};

class ModuleIrcops : public Module
{
 private:
	CommandIrcops cmd;

 public:
	ModuleIrcops()
		: cmd(this)
	{
	}

	Version GetVersion() CXX11_OVERRIDE
	{
		return Version("Comando IRCOPS para listar operadores IRC", VF_VENDOR);
	}
};

MODULE_INIT(ModuleIrcops) 
