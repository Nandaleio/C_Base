<script setup lang="ts">
import {
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogClose,
    Dialog,
    DialogTrigger
} from '@/components/ui/dialog'

import {
    Tooltip,
    TooltipContent,
    TooltipProvider,
    TooltipTrigger,
} from '@/components/ui/tooltip'
import { AlertDialogTrigger, AlertDialog } from '@/components/ui/alert-dialog'
import AlertConfirm from '@/components/AlertConfirm.vue';
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';
import {
    Select,
    SelectContent,
    SelectItem,
    SelectTrigger,
    SelectValue,
} from '@/components/ui/select'
import { Trash2, Settings } from 'lucide-vue-next'
import { ref } from 'vue';
import { cbFetch } from '@/services/api-service';


const props = defineProps<{
    tableName: string,
    cols?: { name: string, type: string }[]
}>()

const column = ref<{ name: string, type: string }[]>([]);
const open = ref();

async function deleteTable() {
    await cbFetch(`/api/admin/table/${props.tableName}`, 'DELETE');

}

const emit = defineEmits<{
    updated: []
    deleted: []
}>()
</script>

<template>

    <Dialog :open="open" @update:open="(o) => open = o">
        <DialogTrigger>
            <TooltipProvider>
                <Tooltip>
                    <TooltipTrigger as-child>
                        <Button variant="outline">
                            <Settings />
                        </Button>
                    </TooltipTrigger>
                    <TooltipContent>
                        <p>Edit table</p>
                    </TooltipContent>
                </Tooltip>
            </TooltipProvider>
        </DialogTrigger>

        <DialogContent>
            <DialogHeader>
                <DialogTitle>Edit the table {{ tableName }}</DialogTitle>
                <DialogDescription>

                </DialogDescription>
            </DialogHeader>

            <div class="flex items-center" v-for="col of column">
                <Input type="text" name="tableName" placeholder="Table Name" v-model="col.name" />
                <Select v-model="col.type">
                    <SelectTrigger>
                        <SelectValue placeholder="Column type" />
                    </SelectTrigger>
                    <SelectContent>
                        <SelectItem value="INTEGER">INTEGER</SelectItem>
                        <SelectItem value="REAL">REAL</SelectItem>
                        <SelectItem value="TEXT">TEXT</SelectItem>
                        <SelectItem value="BLOB">FILE</SelectItem>
                    </SelectContent>
                </Select>
                <Button variant="destructive" @click="column.splice(column.indexOf(col), 1)">
                    <Trash2 />
                </Button>
            </div>

            <Button variant="outline" @click="column.push({ name: '', type: '' })">Add Column</Button>

            <AlertDialog>
                <AlertDialogTrigger>
                    <Button variant="destructive">
                        <Trash2 />
                    </Button>
                    <AlertConfirm @confirm="emit('deleted'); open = false"
                        :description="`This action will delete the table ${tableName}`" :action="deleteTable">
                    </AlertConfirm>
                </AlertDialogTrigger>
            </AlertDialog>

            <DialogFooter>
                <DialogClose as-child>
                    <Button type="button" variant="secondary">
                        Cancel
                    </Button>
                </DialogClose>
                <Button @click="">
                    Save Changes
                </Button>
            </DialogFooter>
        </DialogContent>
    </Dialog>
</template>