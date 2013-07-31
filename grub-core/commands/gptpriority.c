/* gptpriority.c - read the priority of gpt partitions  */
/*
 *  GRUB  --  GRand Unified Bootloader
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <grub/command.h>
#include <grub/dl.h>
#include <grub/disk.h>
#include <grub/partition.h>
#include <grub/gpt_partition.h>
#include <grub/misc.h>
#include <grub/mm.h>
#include <grub/fs.h>
#include <grub/env.h>

GRUB_MOD_LICENSE ("GPLv3+");

static grub_err_t
grub_cmd_gptpriority (grub_command_t cmd __attribute__ ((unused)),
                      int argc, char **args)
{
  grub_disk_t disk;
  grub_partition_t part;
  char priority_str[3]; /* Maximum value 15 */

  if (argc < 2 || argc > 3)
    return grub_error (GRUB_ERR_BAD_ARGUMENT,
                       "gptpriority DISKNAME PARTITIONNUM [VARNAME]");

  /* Open the disk if it exists */
  disk = grub_disk_open (args[0]);
  if (!disk)
    {
      return grub_error (GRUB_ERR_BAD_ARGUMENT,
                         "Not a disk");
    }

  part = grub_partition_probe (disk, args[1]);
  if (!part)
    {
      grub_disk_close (disk);
      return grub_error (GRUB_ERR_BAD_ARGUMENT,
                         "No such partition");
    }

  if (grub_strcmp (part->partmap->name, "gpt"))
    {
      grub_disk_close (disk);
      return grub_error (GRUB_ERR_BAD_PART_TABLE,
                         "Not a GPT partition");
    }

  grub_snprintf (priority_str, sizeof(priority_str), "%" PRIuGRUB_UINT64_T,
                 (part->gpt_attrib >> 48) & 0xfULL);

  if (argc == 3)
    {
      grub_env_set (args[2], priority_str);
      grub_env_export (args[2]);
    }
  else
    {
      grub_printf ("Priority is %s\n", priority_str);
    }

  grub_disk_close (disk);
  return GRUB_ERR_NONE;
}

static grub_command_t cmd;

GRUB_MOD_INIT(gptpriority)
{
  (void) mod;
  cmd = grub_register_command ("gptpriority", grub_cmd_gptpriority,
                               "gptpriority DISKNAME PARTITIONNUM [VARNAME]",
                               "\nDisplays the priority of the specified "
                               "partition from the given disk, and optionally"
                               " stores it in an environment variable.\n\n"
                               "E.g. gptpriority hd2 2 prioA");
}

GRUB_MOD_FINI(gptpriority)
{
  grub_unregister_command (cmd);
}
